#!/usr/bin/python

##########################
#  channon@hawk.iit.edu  #
##########################

# version 1.0

import sys
import time
import os
from os import environ
from os.path import dirname,join, isfile
from glob import glob
from functools import partial
import subprocess
import zmq
import logging
import argparse
import thread
import heapq
import shlex

import pipe


ip = sys.argv[1]
port = sys.argv[2]
procs_to_start = sys.argv[3]

# create connection to power coordinator
# open transport layer to power coordinator
# TCP socket
contextOutDSS=zmq.Context()
DSSout=contextOutDSS.socket(zmq.REQ)

print 'Opening Connection to tcp://%s:%s' % (ip,port)
DSSout.connect('tcp://%s:%s' % (ip,port))

# start process
process = subprocess.Popen(procs_to_start,shell=False)
#   add to VT
cmd_vt = "echo %s > /sys/vt/VT7/pid_01" % process.pid
print "Running %s" % cmd_vt
subprocess.Popen(cmd_vt,shell=False)

# create pipe to process
COORD_PIPE = 'tmp/coordination.pipe'
if not os.path.exists(COORD_PIPE):
    os.mkfifo(COORD_PIPE)
pipein = open(COORD_PIPE, 'r')

fn = './tmp/host' 
print 'creating pipe: %s '%fn
if not os.path.exists(fn):
    os.mkfifo(fn)
pipe_host = os.open(fn,os.O_WRONLY)


while True:
    newEvent = pipein.readline()[:-1] # trim newline character
    print "new Event Found: %s " % newEvent
    # if event has arrived
    if newEvent:
        pause()
        event = newEvent.split()
        event = preprocess_event(event)
        reply=send_event(event)
        postprocess_event(reply)
        resume()

def preprocess_event(event):
    return event

def postprocess_event(event):
    pipe.send_sync_event('%s \n'%event,pipe_host)
    
def give_reply(reply):
    pass

def pause():
    cmd_vt = "echo freeze > /sys/vt/VT7/mode" 
    subprocess.Popen(cmd_vt,shell=False)


def resume():
    cmd_vt = "echo unfreeze > /sys/vt/VT7/mode"
    subprocess.Popen(cmd_vt,shell=False)

        
def send_event(req):
    #request is a list turn to string
    request = ' '.join(req)
    req_bytes=request.encode('utf-8')
    DSSout.send(req_bytes)
    status=DSSout.recv()
    data = status.decode('utf-8')
    print ('reply recieved %s: ' % data)
    return data

        
# while
#    listen
#    VT
#    process evt
#    send reply to proc
#    VT

