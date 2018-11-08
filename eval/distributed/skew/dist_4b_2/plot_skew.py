#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import argparse

#matplotlib.rcParams['ps.useafm'] = True
#matplotlib.rcParams['pdf.use14corefonts'] = True
#matplotlib.rcParams['text.usetex'] = True

def start(case1 = True):
    runs = [4,4]
    nums = -1
    files = ['merge_2.log','merge_4.log']
    #files = ['merge_4.log','merge_2.log']

    for file in files:
        nums+=1
        print nums
        if nums == 0:
            rawF = np.genfromtxt(file, delimiter=',', usecols={0,2,4,6} )
        else:
            rawF = np.genfromtxt(file, delimiter=',', usecols={0,4,8,12} )
            
        #print(rawF)
        #p==rint len(rawF)
        
        for y in reversed(range(4)):
            #print rawF[:,y]
            if case1:
                rawF[:,y] = (rawF[:,y] - rawF[:,0])/1000000
            else:
                rawF[:,y] = (rawF[:,y] - rawF[:,0])/1000

            print rawF[:,y]
            
        colors=['b','r','g','c','m','y','k','coral','steelblue','tan','olive','orchid',
                'darksalmon', 'lightslategrey','steelblue']

        plt.subplot(1,2,nums+1)
        pi_s = ["Pi 1 (master)", "189", "80", "92"]
        
        for x in range(0,4):
            l = pi_s[x]
            if case1:
                plt.plot(range(len(rawF[:,0])),rawF[:,x],c=colors[x-1],label=l)
            else:
                plt.plot(range(len(rawF[:,0])),rawF[:,x]/range(len(rawF[:,0])),c=colors[x-1],label=l)
            
        #plt.axis([-1,100,-20,20])
        #plt.axis([-1,100,-20,20])
        
        if case1:
            plt.legend(fontsize=8,loc='lower left')
            plt.ylabel('Milliseconds', fontsize=14)
            plt.title('Cumulative Clock Skewness')
            plt.xlabel('run (index)', fontsize=14)
        
        else:
            plt.legend(fontsize=8,loc='upper right')
            plt.ylabel('Microseconds', fontsize=14)
            plt.title('Clock Skewness')
            plt.xlabel('time (index of pause/resume)', fontsize=14)
        
        plt.grid(True)
        
    plt.show()

        

        
start(1)
start(0)
