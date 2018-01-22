import time
import zmq
import pprint

def result_collector():
    context = zmq.Context()
    results_receiver = context.socket(zmq.PULL)
    results_receiver.bind("tcp://127.0.0.1:5556")
    collecter_data = {}

    result = results_receiver.recv_json()
    collecter_data[result['consumer']] = result['Value']

    pprint.pprint(collecter_data)

def main():
    result_collector()

if __name__ == '__main__':
    main()

