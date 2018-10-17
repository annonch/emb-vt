#!/bin/bash
#
# Author: Christopher Hannon
# Date: 9-6-18
#
###############################
#
# clear PIDS
#
#
#rm /home/emb-vt/eval/skew/*.log
#
# main
#
#for i in $1 #2 4 8 16
for i in 2 4 8 16
do
    sleep 2
    # rebuild kmodule just in case
    rmmod vtgpio
    cd /home/emb-vt/src
    make
    insmod vtgpio.ko
    cd /home/emb-vt/eval/gtod_benchmark
    # clear dmesg
    sleep 5
    dmesg --clear
    ########################
    ### setup experiment ###
    ########################
    echo "setting up experiment ${i}"
    # declare array
    declare -a pids
    #
    # process 0
    /home/emb-vt/eval/gtod_benchmark/start_paused.sh /home/emb-vt/eval/gtod_benchmark/gtod_loop &
    pids[0]=$!
    echo $! > /sys/vt/VT7/pid_01
    echo $!
    #
    # z = upperbound
    z=$(($i-1))
    #
    # start sleeping procs
    for j in `seq 1 $z`; do
	#
	# get pid variable to be double digits (+1)
	jj=$(($j+1))
	# make jj 2 digits for the pid_02-16 variable
	while [[ ${#jj} -lt 2 ]] ; do
	    jj="0${jj}"
	done
	#
	# add sleeping procs to VT
	sleep 2500 &
	pids[j]=$!
	pid_path="/sys/vt/VT7/pid_${jj}"
	echo $! > ${pid_path}
	echo $!
    done
    ######################
    ### run experiment ###
    ######################
    echo "running experiment ${i}"
    #
    # start all procs
    sleep 1
    kill -CONT ${pids[*]}
    #
    # start pause/resume loop
    while kill -0 ${pids[0]} >/dev/null 2>&1
    do
	echo "freeze" > /sys/vt/VT7/mode
	sleep 1
	echo "unfreeze" > /sys/vt/VT7/mode
	for q in "${pids[@]}"
	do
	    #
	    # removes ns\n and replaces with , save to file
	    cat /proc/${q}/fpt | tr 'ns\n' ' , ' >> /home/emb-vt/eval/skew/skew_${i}.log
	done
	echo ' ' >> /home/emb-vt/eval/skew/skew_${i}.log
	sleep 1
    done
    wait ${pids[0]}
    #
    #######################
    ### exit experiment ###
    #######################
    #
    # saving log files
    #
    echo "saving log files to /home/emb-vt/eval/skew/skew_${i}.log and /home/emb-vt/eval/overhead/overhead_${i}.log"
    # 
    kill -9 ${pids[@]}
    #
    # save dmesg
    dmesg > /home/emb-vt/eval/overhead/overhead_${i}.log
done
#
# Cleanup
# clear pids
for x in `seq 1 16`;
do
    xx=$x
    while [[ ${#xx} -lt 2 ]] ; do
	xx="0${xx}"
    done
    
    echo "0" > /sys/vt/VT7/pid_${xx}
done
echo "------------------------------------------------------------------------------------------------------------------"
echo "make sure to save the log files into a new directory so that rerunning this script does not append to old files."
echo "thank you and goodbye"
echo "------------------------------------------------------------------------------------------------------------------"

