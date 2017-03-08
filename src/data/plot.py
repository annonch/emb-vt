#!/usr/bin/python

import sys
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from math import sqrt
import scipy.stats as ss

matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['pdf.use14corefonts'] = True
matplotlib.rcParams['text.usetex'] = True

def plot_all():
    one_run = np.loadtxt(sys.argv[1])
    two_run = np.loadtxt(sys.argv[2])
    three_run = np.loadtxt(sys.argv[3])
    four_run = np.loadtxt(sys.argv[4])

    print len(one_run)
    print len(two_run)
    print len(three_run)
    print len(four_run)


    a = np.subtract(one_run,two_run)
    b = np.subtract(three_run,four_run)

    a = np.sort(a)
    print a
    b = np.sort(b)
    print b
    a_p = 1. * np.arange(len(a)) / (len(a) - 1)
    b_p = 1. * np.arange(len(b)) / (len(b) - 1)

    # plt.figure()
    plt.subplot(1,2,1)
    font = {'size':'15'}
    matplotlib.rc('font', **font)

    color = ('b','g','r','c','m','y','k','b','g')

    plt.plot(a, a_p,
             color=color[0],
             label=('skewness from h1 to h2'))
    plt.plot(b, b_p,
             color=color[1],
             label=('skewness from h2 to h1'))


    plt.grid(True)
    leg = plt.legend(fancybox=True,loc='upper left')
    leg.get_frame().set_alpha(1)
    plt.xlim([-.75, .75])
    # plt.xticks(range(time_start+1, time_end+1, 2))
    # plt.yticks(range(0, int(bw) + 1, 100))

    plt.ylim([0, 1])
    plt.xlabel('Time in (Seconds)', fontsize=20)
    plt.ylabel('Fraction of Trials', fontsize=20)
    #plt.savefig(output_file, format='eps')

    plt.subplot(1,2,2)
    data_m=[]
    data_m.append(np.mean(a))
    data_m.append(np.mean(b))
    print data_m

    data_sd=[]
    data_sd.append(np.std(a))
    data_sd.append(np.std(b))
    print data_sd

    #x_data=[0 for x in a]
    plt.plot(0,data_m[0],color=color[0],marker='s',markersize=8, label='1 to 2')
    #x_data=[1 for x in a]
    plt.plot(1,data_m[1],color=color[1],marker='v',markersize=8, label='2 to 1')

    ye1 = ss.norm.interval(0.05,loc=data_m[0],scale=data_sd[0])
    ye2 = ss.norm.interval(0.05,loc=data_m[1],scale=data_sd[1])
    yerr1 = (ye1[1]-ye1[0])/2
    yerr2 = (ye2[1]-ye2[0])/2
    plt.errorbar(0,data_m[0],yerr1,color=color[0],ecolor=color[0])
    plt.errorbar(1,data_m[1],yerr2,color=color[1],ecolor=color[1])

    #plt.errorbar([0,1],data_m,yerr=ss.norm.interval(0.95,loc = data_m,scale=data_sd/sqrt(len(a))))


    plt.ylabel('Time in (Seconds)')
    plt.xlabel('Trial')
    leg = plt.legend(fancybox=True,loc='lower left')
    leg.get_frame().set_alpha(1)
    plt.grid(True)

    plt.xlim([-1,2])


    plt.show()

if __name__ == '__main__':
    plot_all()
