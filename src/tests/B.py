#!/usr/bin/python

import signal,os
import time
import sys

RUN_TIME = 60 # seconds
if len(sys.argv) > 1:
    print('hi')
    RUN_TIME = int(sys.argv[1])


def setup_pipe():
    pipe_name = '/tmp/vtime_test.pipe'
    if not os.path.exists(pipe_name):
        os.mkfifo(pipe_name)
    return open(pipe_name, 'r')

def listen(pipe):
    return pipe.readline()

start_time = time.time()
my_pipe = setup_pipe()
#while time.time() < (start_time + RUN_TIME):
while 1:
    A = listen(my_pipe)
    if A != "exit":
        print('Program A time: %sProgram B time: %s \n' % ( A , time.time()))
    else:
        sys.exit(0)