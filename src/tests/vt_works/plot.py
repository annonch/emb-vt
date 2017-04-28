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
    c = sys.argv[1]
    a = np.loadtxt(c, delimiter=',', unpack=True)

    #print a

    index = a[0]
    vt = a[1]
    index2 = a[2]
    normal = a[3]
    #print normal

    # plt.figure()
    #plt.subplot(1,2,1)
    plt.subplot(1,1,1)
    font = {'size':'15'}
    matplotlib.rc('font', **font)

    color = ('b','g','r','c','m','y','k','b','g')

    #plt.subplot(1,2,2)

    plt.subplot(1,1,1)
    plt.title("Clock Differences When Program A is paused for 2 seconds every 2 seconds")

    #x_data=[0 for x in a]
    plt.plot(index,vt,color=color[0],marker='s',markersize=8, label='Time in Process A')
    #x_data=[1 for x in a]
    plt.plot(index2,normal,color=color[2],marker='v',markersize=8, label='Time in Process B')
    #plt.set_ticks(100)

    plt.ylabel('Time in (Seconds)')
    plt.xlabel('Request Index ( every 0.5s)')
    leg = plt.legend(fancybox=True,loc='lower right')
    leg.get_frame().set_alpha(1)

    plt.xlim([0,100])
    plt.ylim([0,100])
    plt.grid(True)

    plt.show()

if __name__ == '__main__':
    plot_all()
