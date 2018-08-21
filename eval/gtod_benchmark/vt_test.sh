#!/bin/bash
echo "kicking off processes in paused state"
/home/emb-vt/eval/gtod_benchmark/start_paused.sh /home/emb-vt/eval/gtod_benchmark/gtod_loop &
PID1=$!
echo $PID1

sleep 150 &
PID2=$!
echo $PID2

sleep 150 &
PID3=$!
echo $PID3

sleep 150 &
PID4=$!
echo $PID4

sleep 150 &
PID5=$!
echo $PID5

sleep 150 &
PID6=$!
echo $PID6

sleep 150 &
PID7=$!
echo $PID7

sleep 150 &
PID8=$!
echo $PID8

sleep 150 &
PID9=$!
echo $PID9

sleep 150 &
PID10=$!
echo $PID10

sleep 150 &
PID11=$!
echo $PID11

sleep 150 &
PID12=$!
echo $PID12

sleep 150 &
PID13=$!
echo $PID13

sleep 150 &
PID14=$!
echo $PID14

sleep 150 &
PID15=$!
echo $PID15

sleep 150 &
PID16=$!
echo $PID16

## DILATE ##
echo "adding Proc to VT"
echo $PID1 > /sys/vt/VT7/pid_01

echo $PID2 > /sys/vt/VT7/pid_02
echo $PID3 > /sys/vt/VT7/pid_03
echo $PID4 > /sys/vt/VT7/pid_04
echo $PID5 > /sys/vt/VT7/pid_05
echo $PID6 > /sys/vt/VT7/pid_06
echo $PID7 > /sys/vt/VT7/pid_07
echo $PID8 > /sys/vt/VT7/pid_08

echo $PID9 > /sys/vt/VT7/pid_09
echo $PID10 > /sys/vt/VT7/pid_10
echo $PID11 > /sys/vt/VT7/pid_11
echo $PID12 > /sys/vt/VT7/pid_12
echo $PID13 > /sys/vt/VT7/pid_13
echo $PID14 > /sys/vt/VT7/pid_14
echo $PID15 > /sys/vt/VT7/pid_15
echo $PID16 > /sys/vt/VT7/pid_16

sleep 2
echo "starting processes"
kill -CONT $PID1 $PID2 $PID3 $PID4 $PID5 $PID6 $PID7 $PID8 $PID9 $PID10 $PID11 $PID12 $PID13 $PID14 $PID15 $PID16

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
    sleep 1
done
wait $PID1
kill -9  $PID2  $PID3  $PID4 $PID5 $PID6 $PID7 $PID8 $PID9 $PID10 $PID11 $PID12 $PID13 $PID14 $PID15 $PID16

