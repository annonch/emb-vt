declare -a arr1

for j in `seq 1 100`; 
do
    
    ${arr1[$j]}=${RANDOM:0-1}
done
