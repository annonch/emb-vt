#!/usr/bin/python


import signal,os
import sys
import time

def setup_pipe():
    pipe_name = '/tmp/vtime_test.pipe'
    if not os.path.exists(pipe_name):
        os.mkfifo(pipe_name)
    pipeout = os.open(pipe_name, os.O_WRONLY)
    return pipeout

def send(msg,pipe):
    msg_n = '%s\n'%msg
    try:
        os.write(pipe, msg_n.encode('utf-8'))
    except BrokenPipeError:
        # exit quietly..
        sys.exit(0)


my_pipe = setup_pipe()
start_time=time.time()
for x in range(100):
    send(str(time.time()-start_time),my_pipe)
    time.sleep(0.5)
send("exit",my_pipe)
sys.exit(0)
