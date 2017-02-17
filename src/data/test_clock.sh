i="0"

while [ $i -lt 1000 ]
do
sleep 1
echo freeze > /sys/vt/VT7/mode
sleep 1
echo unfreeze > /sys/vt/VT7/mode
i=$[$i+1]
done

dmesg > dmesg.log
