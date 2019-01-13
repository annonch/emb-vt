#!/bin/bash
# 
# Author: Christopher Hannon
# Date: 11-13-2018
#
#
########################

declare -a IPs
#IPs[0]="216.47.152.92"
#IPs[1]="216.47.152.189"
#IPs[2]="216.47.152.80"
IPs[0]="192.168.1.158"#216.47.152.92"
IPs[1]="192.168.1.156"#216.47.152.189"
IPs[2]="192.168.1.153"#216.47.152.80"

# declare array
declare -a pids
    
## round robin
declare -a pause_order
pause_order[0]="0"
pause_order[1]="1"
pause_order[2]="2"
pause_order[3]="3"
pause_order[4]="0"
pause_order[5]="1"
pause_order[6]="2"
pause_order[7]="3"
pause_order[8]="0"
pause_order[9]="1"
pause_order[10]="2"
pause_order[11]="3"
pause_order[12]="0"
pause_order[13]="1"
pause_order[14]="2"
pause_order[15]="3"
pause_order[16]="0"
pause_order[17]="1"
pause_order[18]="2"
pause_order[19]="3"
pause_order[20]="0"
pause_order[21]="1"
pause_order[22]="2"
pause_order[23]="3"
pause_order[24]="0"
pause_order[25]="1"
pause_order[26]="2"
pause_order[27]="3"
pause_order[28]="0"
pause_order[29]="1"
pause_order[30]="2"
pause_order[31]="3"
pause_order[32]="0"
pause_order[33]="1"
pause_order[34]="2"
pause_order[35]="3"
pause_order[36]="0"
pause_order[37]="1"
pause_order[38]="2"
pause_order[39]="3"
pause_order[40]="0"
pause_order[41]="1"
pause_order[42]="2"
pause_order[43]="3"
pause_order[44]="0"
pause_order[45]="1"
pause_order[46]="2"
pause_order[47]="3"
pause_order[48]="0"
pause_order[49]="1"
pause_order[50]="2"
pause_order[51]="3"
pause_order[52]="0"
pause_order[53]="1"
pause_order[54]="2"
pause_order[55]="3"
pause_order[56]="0"
pause_order[57]="1"
pause_order[58]="2"
pause_order[59]="3"
pause_order[60]="0"
pause_order[61]="1"
pause_order[62]="2"
pause_order[63]="3"

if [ "$#" -ne 2 ]; then
    echo "Usage: ./leader [numProcs] [saveDir]"
    exit
fi

numPids=$1
p2d=$2

trap 'response' 10
trap 'response2' 12
trap 'cleanup' 1 2 3 6

should_wait=1

cleanup()
{
    echo "cleaning up"
    rm ./local.sh.pid
    kill -9 ${pids[@]}
    rmmod vtgpio
    rm local.sh.pid
    echo "exiting ..."
    exit 1   
}

setup()
{
    echo "setting up experiment"
    #############
    #  Logging  #
    #############
    echo $$ > "./local.sh.pid"
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
    sleep 60
    echo "setting up experiment with ${i} processes"
    #
    # z = upperbound
    z=$(($numPids-1))
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

    echo "SIGCONTing sleeping PIDs"

}

response()
{
    echo " " 
    echo "recieved response (sigusr1) "
    should_wait=0
}

response2()
{
    echo "sigusr2 signal received"
}


collect_logs()
{
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

    # stopping remote procs
    echo "stopping remote procs..."
    for j in "${IPs[@]}"
    do
	echo $j
	ssh -i /home/.ssh/id_ecdsa "root@${j}" "kill -6 `(ssh -i /home/.ssh/id_ecdsa root@${j} cat /home/emb-vt/eval/distributed/local.sh.pid)`"
    done

    echo "collecting remote data ..."
    sleep 2
    i=$numPids
    for j in "${IPs[@]}"
    do
	echo $j
	#scp files from remote to local
	scp -i /home/.ssh/id_ecdsa "root@${j}:/home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log" "/home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}_${j}.log"
    done

    # save dmesg
    dmesg > /home/emb-vt/eval/distributed/overhead/${p2d}/overhead_${i}.log

    for j in "${IPs[@]}"
    do
	echo $j
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

    # remove remote files (easier to use git)
    for j in "${IPs[@]}"
    do
	echo $j
	ssh -i /home/.ssh/id_ecdsa "root@${j}" "rm -r /home/emb-vt/eval/distributed/*/${p2d}"
    done
    rm ./local.sh.pid
    
    echo "------------------------------------------------------------------------------------------------------------------"
    echo "thank you and goodbye"
    echo "------------------------------------------------------------------------------------------------------------------"
}

main_loop()
{
    LOPP=0
    echo ""
    cat /proc/${pids[1]}/fpt | tr 'ns\n' ' , ' >> /home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log
    #done
    echo ' ' >> /home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log

    for j in "${IPs[@]}"
    do  # this is initial 0 FTP
	echo $j
	ssh -i /home/.ssh/id_ecdsa "root@${j}" "kill -10 `(ssh -i /home/.ssh/id_ecdsa root@${j} cat /home/emb-vt/eval/distributed/local.sh.pid)`"
    done

    while [ $LOPP -lt 64 ];
    do
	let LOPP=LOPP+1
	# determine who should pause next
	if [ ${pause_order[$LOPP-1]} -eq 3 ]; then
	   # local
	   echo "freeze" > /sys/vt/VT7/mode
	   sleep 1
	   echo "unfreeze" > /sys/vt/VT7/mode
	else
	    should_wait=1
	    j=${IPs[${pause_order[${LOPP}-1]}]}
	    echo $j
	    ssh -i /home/.ssh/id_ecdsa "root@${j}" "kill -12 `(ssh -i /home/.ssh/id_ecdsa root@${j} cat /home/emb-vt/eval/distributed/local.sh.pid)`"
	fi
		
	# wait for pause to be over
	while [ $should_wait -eq 1 ];
	do
	    sleep .1
	done
	#echo $pids
	cat /proc/${pids[1]}/fpt | tr 'ns\n' ' , ' >> /home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log
	#done
	echo ' ' >> /home/emb-vt/eval/distributed/skew/${p2d}/skew_${i}.log

	#ssh to remote  sig 10
	for j in "${IPs[@]}"
	do
	    echo $j
	    #echo ssh -i /home/.ssh/id_ecdsa "root@${i}" "kill -10 `(ssh -i /home/.ssh/id_ecdsa cat /home/emb-vt/eval/distributed/local.sh.pid)`"       
            ssh -i /home/.ssh/id_ecdsa "root@${j}" "kill -10 `(ssh -i /home/.ssh/id_ecdsa root@${j} cat /home/emb-vt/eval/distributed/local.sh.pid)`"
        done
        sleep 1
    done

    #######################  



}

setup
main_loop
collect_logs
