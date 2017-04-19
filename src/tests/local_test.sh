#!/bin/bash
./start_paused.sh ./print_time.py > log_1.txt &
PID1=$!
./start_paused.sh ./print_time.py > log_2.txt &
PID2=$!
echo $PID1
echo $PID2
## DILATE ##
echo "adding P2 to VT"
echo $PID2 > /sys/vt/VT7/pid_01
sleep 2
kill -CONT $PID1
kill -CONT $PID2
for i in `seq 1 100`;
do
    echo "freezing";
    echo "freeze" > /sys/vt/VT7/mode
    sleep 1
    echo "unfreezing"
    echo "unfreeze" > /sys/vt/VT7/mode
    sleep 1
done
pkill print_time.py
