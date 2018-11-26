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
sleep 10

./start_paused.sh iperf -u -c 192.168.0.2 -b 100M -t 32 -i 1 -y c -x MV > iperf_${1}.log &

echo $! > /sys/vt/VT7/pid_01


sleep 2

kill -CONT $!

while kill -0 $PID1 >/dev/null 2>&1
do
    echo "freezing";
    echo "freeze" > /sys/vt/VT7/mode
    sleep 1
    echo "unfreezing"
    echo "unfreeze" > /sys/vt/VT7/mode
    sleep 1
done
wait $PID1

echo "" > /sys/vt/VT7/pid_01

