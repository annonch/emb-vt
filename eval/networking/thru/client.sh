echo "starting throughput experiment using Iperf"

######################
# rebuilding kmodule #
######################
rmmod vtgpio
cd /home/emb-vt/src
make
insmod vtgpio.ko
cd /home/emb-vt/eval/networking/thru/
# clear dmesg
sleep 5
dmesg --clear
########################
### setup experiment ###
########################

# important sleep until all synced...

sleep 25

for j in `seq 0 53`; do
    

    sleep 1

    # CSV
    #./start_paused.sh iperf -u -c 192.168.0.2 -b 100M -t 32 -i 1 -y c -x MV > iperf_${1}.log &

    # Regular
    #./start_paused.sh iperf3 -u -c 192.168.0.2 -b 3200M -t 32 -i 1  > iperf_${1}.log &
    #./start_paused.sh iperf3 -u -c 192.168.0.2 -b 0 -t 32 -i 1  > iperf_${1}.log &
    #TCP
    ./start_paused.sh iperf3 -c 192.168.0.2 -t 32 -i 1 -f k > iperf_${1}_${j}.log &
    
    PID1=$!
    
    #echo $PID1 > /sys/vt/VT7/pid_01
    
    #echo $PID1
    #ps fj
    
    sleep 2

    kill -CONT $PID1

    #ps fj
    
    #sleep 2
    
    
    while kill -0 $PID1 >/dev/null 2>&1
    do
	#echo "freezing";
	echo "freeze" > /sys/vt/VT7/mode
	sleep 1
	
	#cat /proc/${PID1}/fpt
	#echo "unfreezing"
	echo "unfreeze" > /sys/vt/VT7/mode
	sleep 1
    done
    wait $PID1
    sleep 1
    #echo ' ' > /sys/vt/VT7/pid_01
    
done
