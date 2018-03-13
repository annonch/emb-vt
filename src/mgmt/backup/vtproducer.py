import time
import optparse
import zmq


def producer(opt, consumers_ip_list, sensor_id):
    """
    Args:
    opt.................string
    consumers_ip_list...list of string
    sensor_id...........int
    """
    context = zmq.Context()
    zmq_socket = context.socket(zmq.PUSH)

    for ip in consumers_ip_list:
        zmq_socket.bind(ip)
        # Start your result manager and workers before you start your producers
        work_message = { 'Opt' : opt, 'SensorID' : sensor_id }
        zmq_socket.send_json(work_message)
        print '[*] Producer sending:', time.ctime()


def main():
    """ main """
    parser = optparse.OptionParser()
    parser.add_option(
        '-f', '--file',
        dest='file_name',
        type='string', default='consumers.txt',
        help='''File which contains all the consumers IP e.g. tcp://127.0.0.1:5555.'''
    )
    parser.add_option(
        '-o', '--option',
        dest='opt',
        type='string', default='STOP',
        help='''Option for sending out to all the consumers. e.g. STOP or RESUME Default : STOP.'''
    )
    parser.add_option(
        '-s', '--sensor',
        dest='sensor_id',
        type='int', default=1,
        help='''The ID of the sensor which should return the value. 0 as NULL'''
    )

    options, args = parser.parse_args()

    with open(options.file_name) as temp_file:
        consumers_ip_list = [line.rstrip('\n') for line in temp_file]
    #print options.opt, consumers_ip_list, options.sensor_id
    producer(options.opt, consumers_ip_list, options.sensor_id)

if __name__ == '__main__':
    main()
