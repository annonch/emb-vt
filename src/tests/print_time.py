#!/usr/bin/python
import time

old_time=time.time()
time.sleep(1)

while True:
    new_time= time.time()
    print new_time - old_time
    time.sleep(1)
    old_time = new_time
