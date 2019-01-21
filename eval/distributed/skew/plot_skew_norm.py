#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import argparse
import sys
#matplotlib.rcParams['ps.useafm'] = True
#matplotlib.rcParams['pdf.use14corefonts'] = True
#matplotlib.rcParams['text.usetex'] = True

def start():
    nums = 0
    filef= sys.argv[1]# = ['merge_2.log','merge_4.log']
    #files = ['merge_4.log','merge_2.log']
    #cols_t = int(sys.argv[2])
    rawF = np.genfromtxt(filef, delimiter=',')#, usecols={cols_t-1,cols_t*2 -1, cols_t*3-1,cols_t*4-1} )

    #print rawF
    
    for y in (1,2,3):
        #print rawF[:,y]
        rawF[:,y] = (rawF[:,y] - rawF[:,0])/1000000.0
    rawF[:,0] = (rawF[:,0] - rawF[:,0])/1000000.0

    #print rawF
    
    colors=['b','r','g','c']#,'m','y','k','coral','steelblue','tan','olive','orchid',
                #'darksalmon', 'lightslategrey','steelblue']

    plt.subplot(1,1,1)
    pi_s = ["Reference", "$\pi$1", "$\pi$2", "$\pi$3"]
        
    for x in range(0,4):
        l = pi_s[x]
        plt.plot(range(len(rawF[:,x])),rawF[:,x],c=colors[x],label=l)
        #plt.axis([-1,100,-20,20])
        
    plt.legend(fontsize=8,loc='lower left')
    plt.ylabel('Milliseconds', fontsize=14)
    plt.title('Cumulative Clock Skewness')
    plt.xlabel('run (index)', fontsize=14)
        
    plt.grid(True)
        
    plt.show()
        
start()
