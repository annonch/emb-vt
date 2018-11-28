#!/bin/bash
for filename in ./vt/*; do
    ./parse_iperf.awk "$filename" > ${filename}_p.log
done

for filename in ./bsl/*; do
    ./parse_iperf.awk "$filename" > ${filename}_p.log
done
