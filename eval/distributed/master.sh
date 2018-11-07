#!/bin/bash
#
# Author: Christopher Hannon
# Date: 10-17-18
#
###############################
#
#
#
#  Usage: ./local [numProcs] [saveDir] [numBoards]
#
#  Number of procs is $1
#  Log folder is $2
#
#  Hardcodded list of IPs
#
declare -a IPs
IPs[0]="216.47.152.92"
IPs[1]="216.47.152.189"
IPs[2]="216.47.152.80"

#


if [ "$#" -ne 3 ]; then
    echo "Usage: ./local [numProcs] [saveDir] [numBoards]"
    exit
fi
#echo $$ > "./${0}.pid"
numPids=$1
p2d=$2

trap 'cleanup' 1 2 3 6

cleanup()
{
    echo "catching exit ..."
    #rm -f "./${0}.pid"
    kill -9 ${pids[@]}
    exit 1
}

# main
#
for i in $1 #2 4 8 16
do
    #############
    #  Logging  #
    #############
    if [ -d "./overhead/${p2d}" ]; then
	# Control will enter here if $DIRECTORY exists.
	echo "overhead directory exists possibly overwriting ..."
    fi
    if [ -d "./skew/${p2d}" ]; then
	# Control will enter here if $DIRECTORY exists.
	echo "overhead directory exists possibly overwriting ..."
    fi
    if [ ! -d "./overhead/${p2d}" ]; then
	# Control will enter here if $DIRECTORY exists.
	mkdir "./overhead/${p2d}"
    fi
    if [ ! -d "./skew/${p2d}" ]; then
	# Control will enter here if $DIRECTORY exists.
	mkdir "./skew/${p2d}"
    
    fi
    sleep 2
    #
    ######################
    # rebuilding kmodule #
    ######################
    rmmod vtgpio
    cd /home/emb-vt/src
    make
    insmod vtgpio.ko
    cd /home/emb-vt/eval/distributed
    # clear dmesg
    sleep 5
    dmesg --clear
    ########################
    ### setup experiment ###
    ########################
    echo "setting up experiment with ${i} processes"
    # declare array
    declare -a pids
    #
    # z = upperbound
    z=$(($i-1))
    #
    # start sleeping procs
    for j in `seq 0 $z`; do
	#
	# get pid variable to be double digits (+1)
	jj=$(($j+1))
	# make jj 2 digits for the pid_01-16 variable
	while [[ ${#jj} -lt 2 ]] ; do
	    jj="0${jj}"
	done
	#
	# add sleeping procs to VT
	sleep 25000 &
	pids[j]=$!
	pid_path="/sys/vt/VT7/pid_${jj}"
	echo $! > ${pid_path}
	echo $!
    done
    ######################
    ### run experiment ###
    ######################
    echo "entering wait loop"
    #
    # start all procs
    sleep 1
    kill -CONT ${pids[*]}
    #
    #while kill -0 ${pids[0]} >/dev/null 2>&1
    LOPP=0
    while [ $LOPP -lt 100 ];
    do
	let LOPP=LOPP+1
	echo "freeze" > /sys/vt/VT7/mode
	sleep 1
	echo "unfreeze" > /sys/vt/VT7/mode
	#echo "getting FPTs ..."
	for q in "${pids[@]}"
	do
      	    # removes ns\n and replaces with , save to file
	    cat /proc/${q}/fpt | tr 'ns\n' ' , ' >> /home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log
	done
	echo ' ' >> /home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log

	#ssh to remote  sig 10
	for j in "${IPs[@]}"
	do
	    #echo ssh -i /home/.ssh/id_ecdsa "root@${i}" "kill -10 `(ssh -i /home/.ssh/id_ecdsa cat /home/emb-vt/eval/distributed/local.sh.pid)`"
	    ssh -i /home/.ssh/id_ecdsa "root@${j}" "kill -10 `(ssh -i /home/.ssh/id_ecdsa root@${j} cat /home/emb-vt/eval/distributed/local.sh.pid)`"
	    # do whatever on $i
	done 

	sleep 1
    done

    #######################
    ### exit experiment ###
    #######################
    #
    # saving log files
    #
    echo "saving log files to /home/emb-vt/eval/distributed/[skew\overhead]/${p2d}/[skew\overhead]_${i}.log"
    # 
    kill -9 ${pids[@]}
    #

    for j in "${IPs[@]}"
    do
	ssh -i /home/.ssh/id_ecdsa "root@${j}" "kill -12 `(ssh -i /home/.ssh/id_ecdsa root@${j} cat /home/emb-vt/eval/distributed/local.sh.pid)`"
	# do whatever on $i
	
    done

    echo "collecting remote data ..."
    sleep 5

    for j in "${IPs[@]}"
    do
	#scp files from remote to local
	scp -i /home/.ssh/id_ecdsa "root@${j}:/home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log" "/home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}_${j}.log"
    done
    
    # save dmesg
    dmesg > /home/emb-vt/eval/distributed/overhead/${p2d}/overhead_${i}.log

    for j in "${IPs[@]}"
    do
	#scp files from remote to local
	scp -i /home/.ssh/id_ecdsa "root@${j}:/home/emb-vt/eval/distributed/overhead/${p2d}/overhead_${i}.log" "/home/emb-vt/eval/distributed/overhead/${p2d}/overhead_${i}_${j}.log"
    done
    
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
    echo "thank you and goodbye"
    echo "------------------------------------------------------------------------------------------------------------------"
    
done
