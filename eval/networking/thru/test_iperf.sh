#!/bin/bash

SWs="0"
PerfTime=32
#bw="800" # mb
REPEAT=50

Durations="1.0" # seconds
Intervals="1.0" # 0.5 0.1 0.05" # seconds

python avg_plot.py $REPEAT iPerfRateAvg.eps #${bw}

