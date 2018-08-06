#!/bin/bash
echo "kicking off processes in paused state"
/home/emb-vt/eval/gtod_benchmark/start_paused.sh /home/emb-vt/eval/gtod_benchmark/gtod_loop &
PID1=$!
echo $PID1
## DILATE ##
echo "adding Proc to VT"
echo $PID1 > /sys/vt/VT7/pid_01
sleep 4
echo "starting processes"
kill -CONT $PID1

while kill -0 $PID1 >/dev/null 2>&1
do
    echo "freezing";
    #START = 'date +"%T.%N"'
    #time
    #kill -STOP $PID1 
    echo "freeze" > /sys/vt/VT7/mode
    #END = date +"%T.%N"
    #echo Time taken to freeze: $((END - START))
    sleep 1
    echo "unfreezing"
    #START = date +"%T.%N"
    #time
    echo "unfreeze" > /sys/vt/VT7/mode
    #kill -CONT $PID1
    #END = date +"%T.%N"
    #echo Time taken to freeze: $((END - START))
    sleep .5
    cat /sys/vt/VT7/OHseconds >> ./PAUSE_LOG.txt
    echo ';' >> ./PAUSE_LOG.txt
    cat /sys/vt/VT7/OHns >> ./PAUSE_LOG.txt
    echo '\n' >> ./PAUSE_LOG.txt
    cat /sys/vt/VT7/OH_R_seconds >> ./RESUME_LOG.txt
    echo ';' >> ./PAUSE_LOG.txt
    cat /sys/vt/VT7/OH_R_ns >> ./RESUME_LOG.txt
    echo '\n' >> ./PAUSE_LOG.txt
    sleep .5
done
wait $PID1
