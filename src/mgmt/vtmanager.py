import time
import random
import multiprocessing
import zmq


class HostService(object):
    """class of service handling"""
    def __init__(self, node_id, node_value, proc_status):
        """
        Args:
        noe_id............int
        node_value........list of float
        proc_status.......dict {123:True, 124:False}  bool true unfreeze false freeze
        """
        self.node_id = node_id
        self.node_value = node_value
        self.proc_status = proc_status # no more than 16

    def getValue(self):
        return self.node_value

    def setValue(self, node_value):
        self.node_value = node_value

    def getID(self):
        return self.node_id

    def getStatus(self):
        return self.proc_status

    def setStatus(self, proc_status):
        self.proc_status = proc_status


class ControllerHandler(HostService):
    """Class to handle behavior of Controller"""
    def __init__(self, type_of_service, node_id, node_value, proc_status):
        """
        Args:
        type_of_service...int 1. Controller 2. Sensor
        noe_id............int
        node_value........list of float
        proc_status.......dict {123:True, 124:False}  bool true unfreeze false freeze
        """
        HostService.__init__(self, node_id, node_value, proc_status)
        self.type_of_service = type_of_service

    def getFunc(self):
        """ Return a string that contains name of Function...string"""
        return "valueRetriever" # should be different


class SensorHandler(HostService):
    """Class to handle behavior of Sensor"""
    def __init__(self, type_of_service, node_id, node_value, proc_status):
        """
        Args:
        type_of_service...int 1. Controller 2. Sensor
        noe_id............int
        node_value........list of float
        proc_status.......dict {123:True, 124:False}  bool true unfreeze false freeze
        """
        HostService.__init__(self, node_id, node_value, proc_status)
        self.type_of_service = type_of_service

    def getFunc(self):
        """ Return a string that contains name of Function...string"""
        return "valueRetriever"


class ConnectionManager(object):
    """Class to send command"""
    def __init__(self):
        self.context = zmq.Context()
        self.zmq_socket = self.context.socket(zmq.PAIR)

        # File which contains all the consumers IP e.g. tcp://127.0.0.1:5555.
        with open('consumers.txt') as temp_file:
            self.consumers_ip_list = [line.rstrip('\n') for line in temp_file]

    def sendCommand(self, opt, server_localhost):
        """
        Args:
        opt................operation e.g. STOP RESUME
        server_localhost...HostService
        """
        for ip in self.consumers_ip_list:
            self.zmq_socket.connect(ip)
            # Start your result manager and workers before you start your producers
            work_message = { 'Opt' : opt, 'FromID' : str(server_localhost.getID()) }
            self.zmq_socket.send_json(work_message) # remove for testing

        print '[*] Sending', opt, ' :', time.ctime()

    def processHandler(self, opt, server_localhost):
        """
        write freeze or unfreeze to /sys/vt/VT7/mode
        Args:
        opt...string
        server_localhost...HostService
        """
        cp_proc_dict = server_localhost.getStatus()

        for key, _ in  cp_proc_dict.items():
            if opt == 'STOP':
                cp_proc_dict[key] = False
            elif opt == 'RESUME':
                cp_proc_dict[key] = True

     # update the dict for class
        server_localhost.setStatus(cp_proc_dict)

        if opt == 'STOP':
            f2 = open('/sys/vt/VT7/mode', 'w')
            f2.write('freeze')
            f2.close()
            print '[*] Stop Services', time.ctime()

        elif opt == 'RESUME':
            f2 = open('/sys/vt/VT7/mode', 'w')
            f2.write('unfreeze')
            f2.close()
            print '[*] Resume Services', time.ctime()


""" Connection Manager """

def writeProcToFile(server_localhost):
    """
    wrtie PIDs to /sys/vt/VT7/pid_...
    Args:
    server_localhost...HostService
    """
    cp_proc_dict = server_localhost.getStatus()

    for i, (key, _) in  enumerate(cp_proc_dict.items()):
        file_name = '/sys/vt/VT7/pid_' + str(i+1).zfill(2) # pid file starts from 01 not 00
        f1 = open(file_name, 'w')
        f1.write(str(key))
        f1.close()


def startConnectionManager(server_localhost):
    """
    start the service for listening
    Arg:
    server_localhost...HostService
    """
    connectionMg = ConnectionManager()
    # TODO: Correct the IPs
    context = zmq.Context()
    # recieve work
    socket = context.socket(zmq.PAIR)
    socket.bind("tcp://127.0.0.1:5555") # dummy data

    while True:
        work = socket.recv_json()
        opt = work['Opt']
        from_sender = work['FromID']
        # target_ids_list = work['NodeIDs']
        # handling stop or resume process
        print '[*] Receive', opt, ' From :', from_sender, '  ', time.ctime()
        if __debug__:
            if opt == 'STOP':
                time.sleep(2)
                connectionMg.processHandler('RESUME', server_localhost)
                print '[*] Resume: ', time.ctime()
        else:
            pass

""" Host Activities """

def getSensorData():
    """
    Get the values from sensors
    """
    # get random value for now. to simulate getting value from real sensors
    ret_val = random.randrange(0, 10)
    return ret_val


def valueRetriever(server_localhost):
    """
    start the service for retrieving data and handling pause
    Arg:
    server_localhost...HostService
    """
    connectionMg = ConnectionManager()
    while True:
        time.sleep(1)
        """ # take out this part for loopback
        while (False in server_localhost.getStatus().values()): # to lock the proc while in the stop state
            time.sleep(1)
            print '[*] Retrieving Pause'
        """
        sensor_val = getSensorData()
        print '[*] Value: ', sensor_val
        if __debug__:
           if sensor_val < 2: # assume this is the situation we need to pause the system
                print '[*] Not getting value, system pasuing'
                connectionMg.sendCommand('STOP', server_localhost)
                connectionMg.processHandler('STOP', server_localhost)
        else:
            if sensor_val < 2: # assume this is the situation we need to pause the system
                print '[*] Not getting value, system pasuing'
                connectionMg.sendCommand('STOP', server_localhost)
                connectionMg.processHandler('STOP', server_localhost)

                #  TODO: Shoudl have a lock here, wait until you get the value
                while sensor_val < 2:
                    time.sleep(1)
                    sensor_val = getSensorData()
                    print '[*] Requesting updated Value: ', sensor_val

                connectionMg.sendCommand('STOP', server_localhost)
                connectionMg.processHandler('STOP', server_localhost)

            server_localhost.setValue([sensor_val])


def main():
    """main"""
    if __debug__:
        print 'Debug mode: ON'
    else:
        print 'Debug mode: OFF'

    try:
        type_of_service = int(raw_input('Please selete the type of service: \n1. Controller \
                                        \n2. Sensor \nPress Enter: '))
    except ValueError:
        print "Error: Please confirm your input!"

    try:
        node_id = int(raw_input('Please enter the ID for this node: \nPress Enter: '))
    except ValueError:
        print "Error: Please confirm your input!"

    # for now both of them do the same thing
    if type_of_service == 1 or type_of_service == 2:
        # add real value
        if type_of_service == 1:
            server_localhost = ControllerHandler(type_of_service, node_id, [0.], {0:True}) # init
            print "[*] Starting Controller"
        elif type_of_service == 2:
            server_localhost = SensorHandler(type_of_service, node_id, [0.], {0:True}) # init
            print "[*] Starting Sensor"

        print "[*] Starting Service ID: #%s" % (server_localhost.getID())
        p1 = multiprocessing.Process(name='p1', target=startConnectionManager, args=(server_localhost,))
        p1.start()
        print "[*] Connection Manager Started..."

        func_for_eval = server_localhost.getFunc()
        p2 = multiprocessing.Process(name='p2', target=eval(func_for_eval), args=(server_localhost,))
        p2.start()
        # Get p2.pid AKA the pid for the node and put it in the class
        server_localhost.setStatus({int(p2.pid):True})
        writeProcToFile(server_localhost) # take out this part for testing
        print "[*] Set PID: ", p2.pid, " to proc dict"
        print "[*] Host Activities Started..."
    else:
        print "Error: Please confirm your input"


if __name__ == '__main__':
    main()
