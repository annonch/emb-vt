#!/bin/bash
echo "kicking off processes in paused state"
./start_paused.sh ./print_time.py > log_1.txt &
PID1=$!
./start_paused.sh ./print_time.py > log_2.txt &
PID2=$!
echo $PID1
echo $PID2
## DILATE ##
echo "adding P2 to VT"
echo $PID2 > /sys/vt/VT7/pid_01
sleep 10
echo "starting processes"
kill -CONT $PID1
kill -CONT $PID2
for i in `seq 1 100`;
while [ps -p $PID1 > /dev/null]
    echo "freezing";
    echo "freeze" > /sys/vt/VT7/mode
    sleep 10
    echo "unfreezing"
    echo "unfreeze" > /sys/vt/VT7/mode
    sleep 10
done
wait $PID1
wait $PID2
