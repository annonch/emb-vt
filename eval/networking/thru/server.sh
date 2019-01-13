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
sleep 1
dmesg --clear
########################
### setup experiment ###
########################

sleep 1

#./start_paused.sh iperf3 -u -s & #c 192.168.0.2 -b 100M -t 32 -i 1 -y c -x MV > iperf_${1}.log &
# no -u
./start_paused.sh iperf3 -s & 

PID1=$!

#echo $PID1 > /sys/vt/VT7/pid_01

sleep 1

kill -CONT $PID1

wait $PID1

#echo "" > /sys/vt/VT7/pid_01

