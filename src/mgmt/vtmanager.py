import time
import random
import multiprocessing
import zmq


class HostService(object):
    """class of service handling"""
    def __init__(self, type_of_service, node_id, node_value, proc_status):
        """
        Args:
        type_of_service...int 1. Controller 2. Sensor
        noe_id............int
        node_value........list of float
        proc_status.......dict {123:True, 124:False}  bool true unfreeze false freeze
        """
        self.type_of_service = type_of_service
        self.node_id = node_id
        self.node_value = node_value
        self.proc_status = proc_status

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


def process_handler(opt, server_localhost):
    """
    Args:
    opt...string
    server_localhost...HostService
    """
    # TODO: Write PIDs into files
    """
    for key, val in server_localhost.getStatus:
        f1 = open('/sys/vt/VT7/pid_01', 'w')
        f1.write(str(pid))
        f1.close()
    """
    pass
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


def send_command(opt, server_localhost):
    """
    Args:
    opt................operation e.g. STOP RESUME
    server_localhost...HostService
    """
    context = zmq.Context()
    zmq_socket = context.socket(zmq.PUSH)

    # File which contains all the consumers IP e.g. tcp://127.0.0.1:5555.
    with open('consumers.txt') as temp_file:
        consumers_ip_list = [line.rstrip('\n') for line in temp_file]

    for ip in consumers_ip_list:
        zmq_socket.bind(ip)
        # Start your result manager and workers before you start your producers
        work_message = { 'Opt' : opt, 'FromID' : server_localhost.getID }
        zmq_socket.send_json(work_message) # remove for testing

    print '[*] Sending', opt, ' :', time.ctime()


def start_listener(server_localhost):
    """
    start the service for listening
    Arg:
    server_localhost...HostService
    """
    # TODO: Correct the IPs
    context = zmq.Context()
    # recieve work
    consumer_receiver = context.socket(zmq.PULL)
    consumer_receiver.connect("tcp://127.0.0.1:5555") # dummy data

    while True:
        work = consumer_receiver.recv_json()
        opt = work['Opt']
        from_sender = work['FromID']
        # target_ids_list = work['NodeIDs']
        # handling stop or resume process
        print '[*] Receive', opt, ' From :', from_sender, '  ', time.ctime()
        process_handler(opt, server_localhost)


def get_sensor_data():
    """
    Get the values from sensors
    """
    # get random value for now. to simulate getting value from real sensors
    ret_val = random.randrange(0, 20)
    return ret_val


def value_retriever(server_localhost):
    """
    start the service for retrieving data and handling pause 
    Arg:
    server_localhost...HostService
    """

    while True:
        time.sleep(1)
        sensor_val = get_sensor_data()
        print '[*] Value: ', sensor_val 
        if sensor_val < 2: # assume this is the situation we need to pause the system
            print '[*] Not getting value, system pasuing'
            send_command('STOP', server_localhost)           

            # wait until you get the value
            while sensor_val < 2:
                time.sleep(1)
                sensor_val = get_sensor_data()
                print '[*] Requesting updated Value: ', sensor_val 
            send_command('RESUME', server_localhost)   
              
        server_localhost.setValue([sensor_val])


def main():
    """main"""

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
        server_localhost = HostService(type_of_service, node_id, [0.], {0:True}) # init
        print "[*] Starting Service ID: #%s" % (node_id)
        p1 = multiprocessing.Process(name='p1', target=start_listener, args=(server_localhost,))
        p2 = multiprocessing.Process(name='p2', target=value_retriever,  args=(server_localhost,))
        p1.start()
        print "[*] Listening..."
        p2.start()
        print "[*] Retrieving..."
    else:
        print "Error: Please confirm your input"


if __name__ == '__main__':
    main()
