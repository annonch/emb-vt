import time
import pprint
import optparse
import zmq
import vtproducer


def result_collector():
    """result_collector"""
    context = zmq.Context()
    results_receiver = context.socket(zmq.PULL)
    results_receiver.bind("tcp://127.0.0.1:5556")
    collecter_data = {}

    result = results_receiver.recv_json()
    collecter_data[result['consumer']] = result['Value']
    print '[*] Get result:'
    pprint.pprint(collecter_data)


def main():
    """main"""
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
        type='string', default='RESUME',
        help='''Option for sending out to all the consumers. e.g. STOP or RESUME Default : STOP.'''
    )

    options, args = parser.parse_args()

    with open(options.file_name) as temp_file:
        consumers_ip_list = [line.rstrip('\n') for line in temp_file]

    result_collector()

    # send back reusme
    # Args:
    # opt.................string
    # consumers_ip_list...list of string
    # sensor_id...........int
    vtproducer.producer(options.opt, consumers_ip_list, 0)
    print '[*] Resume:', time.ctime()

if __name__ == '__main__':
    main()
