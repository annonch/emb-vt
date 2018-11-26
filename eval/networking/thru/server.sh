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

sleep 1

./start_paused.sh iperf -u -s & #c 192.168.0.2 -b 100M -t 32 -i 1 -y c -x MV > iperf_${1}.log &

echo $! > /sys/vt/VT7/pid_01

sleep 2

kill -CONT $!

wait $PID1

#echo "" > /sys/vt/VT7/pid_01

