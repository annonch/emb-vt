import time
import zmq


class NetworkService(object):
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

    def getStatus(self):
        return self.proc_status

    def setStatus(self, proc_status):
        self.proc_status = proc_status


def process_handler(opt, pid):
    """
    Arg:
    opt...string
    pid...int
    """
    if opt == 'STOP':
        f1 = open('/sys/vt/VT7/pid_01', 'w')
        f1.write(str(pid))
        f1.close()
        f2 = open('/sys/vt/VT7/mode', 'w')
        f2.write('freeze')
        f2.close()
        print '[*] Stop Services', time.ctime()

    elif opt == 'RESUME':
        f2 = open('/sys/vt/VT7/mode', 'w')
        f2.write('unfreeze')
        f2.close()
        print '[*] Resume Services', time.ctime()


def start_controller(node_id):
    """
    start the service for controller
    Arg:
    node_id...int
    """

    print "[*] Starting Controller #%s" % (node_id)
    # TOD: add real value
    controller_ser = NetworkService(1, node_id, 0, {123:True})

    # TODO: Correct the IPs
    context = zmq.Context()
    # recieve work
    consumer_receiver = context.socket(zmq.PULL)
    consumer_receiver.connect("tcp://127.0.0.1:5555")
    # send work
    consumer_sender = context.socket(zmq.PUSH)
    consumer_sender.connect("tcp://127.0.0.1:5556")

    while True:
        work = consumer_receiver.recv_json()
        opt = work['Opt']
        target_ids_list = work['NodeIDs']

        # handling stop or resume process
        # process_handler(opt, pid)

        if opt == 'STOP' and node_id in target_ids_list:
            # dummy data for now
            ret_vals = [1, 2, 3]
            result = {'SenderID' : node_id, 'Opt' : opt, 'Values' : ret_vals}
            print '[*] Sending:', result, time.ctime()
            #consumer_sender.send_json(result)



def strat_sensor(node_id):
    """
    start the service for sensor
    Arg:
    node_id...int
    """
    pass


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


    if type_of_service == 1:
        start_controller(node_id)
    elif type_of_service == 2:
        strat_sensor(node_id)
    else:
        print "Error: Please confirm your input"


if __name__ == '__main__':
    main()
