import time
import random
import optparse
import zmq


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


def consumer(consumer_id, master_ip, pid):
    """
    Args:
    consumer_id...int
    master_ip.....string
    pid...........int
    """
    # id should be different from sensors to sensors
    print "[*] Consumer #%s" % (consumer_id)
    context = zmq.Context()
    # recieve work
    consumer_receiver = context.socket(zmq.PULL)
    consumer_receiver.connect("tcp://127.0.0.1:5555")
    # send work
    consumer_sender = context.socket(zmq.PUSH)
    consumer_sender.connect(master_ip)

    while True:
        work = consumer_receiver.recv_json()
        opt = work['Opt']
        sensor_id = work['SensorID']

        # handling stop or resume process
        process_handler(opt, pid)

        if opt == 'STOP' and consumer_id == sensor_id:
            # TODO: return the real sensor value
            ret_val = random.randrange(0, 10000)
            result = {'consumer' : consumer_id, 'Opt' : opt, 'Value' : ret_val}
            print '[*] Sending:', result, time.ctime()
            consumer_sender.send_json(result)


def main():
    """main"""
    parser = optparse.OptionParser()
    parser.add_option(
        '-i', '--id',
        dest='consumer_id',
        type='int', default=1,
        help='''ID of this sensor'''
    )
    parser.add_option(
        '-m', '--masterip',
        dest='master_ip',
        type='string', default='tcp://127.0.0.1:5556',
        help='''IP of the Master Controller'''
    )
    parser.add_option(
        '-p', '--pid',
        dest='pid',
        type='int', default=0,
        help='''PID'''
    )

    options, args = parser.parse_args()
    consumer(options.consumer_id, options.master_ip, options.pid)

if __name__ == '__main__':
    main()
