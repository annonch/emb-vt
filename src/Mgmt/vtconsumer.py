import time
import zmq
import random
import optparse

def consumer(consumer_id, master_ip):
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
        data = work['Opt']
        sensor_id = work['SensorID']
        # TODO: return the real sensor value
        ret_val = random.randrange(0, 10000)
        result = { 'consumer' : consumer_id, 'Opt' : data, 'Value' : ret_val}
        print result

        if consumer_id == sensor_id:
            consumer_sender.send_json(result)

def main():
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
        help='''ID of this sensor'''
    )

    options, args = parser.parse_args()
    consumer(options.consumer_id, options.master_ip)

if __name__ == '__main__':
    main()