#!/bin/bash
#
# Author: Christopher Hannon
# Date: 9-6-18
#
for i in 2 4 8 16
do
    # clear dmesg
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
    #
    # z = upperbound
    z=$(($i-1))
    #
    # start sleeping procs
    for j in seq `1 $z`; do
	#
	# get pid variable to be double digits (+1)
	jj=$j
	# make jj 2 digits for the pid_02-16 variable
	while [[ ${#jj} -lt 2 ]] ; do
	    jj="0${jj}"
	done
	#
	# add sleeping procs to VT
	sleep 150 &
	pids[j]=$!
	pid_path="/sys/vt/VT7/pid_${jj}"
	echo $! > pid_path
    done
    ######################
    ### run experiment ###
    ######################
    echo "running experiment ${i}"
    #
    # start all procs
    kill -CONT ${pids[*]}
    #
    # start pause/resume loop
    while kill -0 ${pids[0]} >/dev/null 2>&1
    do
	echo "freeze" > /sys/vt/VT7/mode
	sleep 1
	echo "unfreeze" > /sys/vt/VT7/mode
	for j in "${pids}"
	do
	    #
	    # removes ns\n and replaces with , save to file
	    cat /proc/${j} | tr 'ns\n' ',' >> skew_${i}.log
	done
	echo "\n" >> skew.log
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
    dmesg > overhead_${i}.log
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
    echo " " > /sys/vt/VT7/pid_${xx}
done
echo "------------------------------------------------------------\nmake sure to save the log files into a new directory so that rerunning this script does not append to old files."
echo "thank you and goodbye\n-------------------------------------------------------------\n"

