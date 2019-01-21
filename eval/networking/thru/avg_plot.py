#!/usr/bin/python

import sys
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from math import sqrt

matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['pdf.use14corefonts'] = True
matplotlib.rcParams['text.usetex'] = True

def plot_all():
    bsl = []
    vir = []
    for i in range(0, num_files):
        one_run = np.loadtxt("./bsl/iperf_tcp_no_vt_%d.log_p.log" % i)
        bsl.append(one_run)

        one_run = np.loadtxt("./vt/iperf_tcp_vt_%d.log_p.log" %i )
        vir.append(one_run)

    bsl = [x/1000.0 for x in bsl]
    vir = [x/1000.0 for x in vir]
    bsl_avg = np.average(bsl, 0)
    bsl_std = np.std(bsl, 0)
    bsl_conf_int = bsl_std / sqrt(1.0 * len(bsl_std)) * 2.58
    vir_avg = np.average(vir, 0)
    vir_std = np.std(vir, 0)
    vir_conf_int = vir_std / sqrt(1.0 * len(vir_std)) * 2.58

    time_start = 0
    time_end = 30

    x_data = range(time_start, time_end)

    print bsl_avg
    print bsl_std
    print vir_avg
    print vir_std
    print x_data

    plt.figure()
    font = {'size':'17'}
    matplotlib.rc('font', **font)
    plt.plot(x_data, bsl_avg, color='blue', marker='s', markersize=8,
             linewidth=1.0, label='Baseline (no Pause/Resume)')
    plt.plot(x_data, vir_avg, color='red', marker='v', markersize=8,
             linewidth=1.0, label='Pause Duration=1 s, Interval=1 s')

    plt.errorbar(x_data, bsl_avg, bsl_std, color='blue', ecolor='blue')
    plt.errorbar(x_data, vir_avg, vir_std, color='red', ecolor='red')
    plt.grid(True)
    plt.legend(fontsize=14,loc='lower right')
    plt.xlim([time_start, time_end-1])
    plt.xticks(range(time_start+1, time_end+1, 2))
    plt.yticks(range(0, int(bw) + 1, 100))
    plt.ylim([200, bw])
    plt.xlabel('Time (Seconds)', fontsize=20)
    plt.ylabel('iperf Throughput (Mbps)', fontsize=20)
    plt.savefig(output_file, format='eps')
    plt.show()
    
if __name__ == '__main__':
    num_files = int(sys.argv[1])
    output_file = sys.argv[2]
    bw = 600 #int(sys.argv[3])
    plot_all()

