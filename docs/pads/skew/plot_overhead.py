

#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import argparse

matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['pdf.use14corefonts'] = True
matplotlib.rcParams['text.usetex'] = True







#############################################################################################
# 
# 1) read in files
#
# 2) split RESUME and PAUSE / UNFREEZE and FREEZE
# 
# 3) cdfs for each 2x5
#
# 4) time x # processes with confidence intervals
#
#############################################################################################


def start():

    single_board_files = ['./single_fin_all/overhead_1.log',
                          './single_fin_all/overhead_2.log',
                          './single_fin_all/overhead_4.log',
                          './single_fin_all/overhead_8.log',
                          './single_fin_all/overhead_16.log']

    dist_2_board_files = [['dist_fin_01_2/overhead_1.log' , 'dist_fin_01_2/overhead_1_192.168.1.158.log' ],
                          ['dist_fin_02_2/overhead_2.log' , 'dist_fin_02_2/overhead_2_192.168.1.158.log' ],
                          ['dist_fin_04_2/overhead_4.log' , 'dist_fin_04_2/overhead_4_192.168.1.158.log' ],
                          ['dist_fin_08_2/overhead_8.log' , 'dist_fin_08_2/overhead_8_192.168.1.158.log' ]]

    dist_4_board_files = [['dist_fin_01/overhead_1.log'  , 'dist_fin_01/overhead_1_192.168.1.158.log'  , 'dist_fin_01/overhead_1_192.168.1.153.log'  , 'dist_fin_01/overhead_1_192.168.1.156.log'  ],
                          ['dist_fin_02/overhead_2.log'  , 'dist_fin_02/overhead_2_192.168.1.158.log'  , 'dist_fin_02/overhead_2_192.168.1.153.log'  , 'dist_fin_02/overhead_2_192.168.1.156.log'  ],
                          ['dist_fin_04/overhead_4.log'  , 'dist_fin_04/overhead_4_192.168.1.158.log'  , 'dist_fin_04/overhead_4_192.168.1.153.log'  , 'dist_fin_04/overhead_4_192.168.1.156.log'  ]]

    single_board_pause  = []
    single_board_resume = []
    dist_2_board_pause  = []
    dist_2_board_resume = []
    dist_4_board_pause  = []
    dist_4_board_resume = []
    
    # get single files
    arr_indx = 0
    for f in single_board_files:
        
        rawF = np.genfromtxt(f, delimiter=';', usecols=2 )
        
        single_board_pause.append([])
        single_board_resume.append([])
        
        file_indx = 1
        for  line in rawF:
            file_indx += 1
            if file_indx % 2 == 0 :
                single_board_pause[arr_indx].append(line/1000)
            else:
                single_board_resume[arr_indx].append(line/1000)
        arr_indx += 1

    # get 2 board files
    arr_indx = 0
    for ff in dist_2_board_files:
        dist_2_board_pause.append([])
        dist_2_board_resume.append([])
        
        for f in ff:
            rawF = np.genfromtxt(f, delimiter=';', usecols=2 )

            file_indx = 1
            for  line in rawF:
                file_indx += 1
                if file_indx % 2 == 0 :
                    dist_2_board_pause[arr_indx].append(line/1000)
                else:
                    dist_2_board_resume[arr_indx].append(line/1000)
        #inside first for loop
        arr_indx += 1

    # get 2 board files
    arr_indx = 0
    for ff in dist_4_board_files:
        dist_4_board_pause.append([])
        dist_4_board_resume.append([])
        
        for f in ff:
            rawF = np.genfromtxt(f, delimiter=';', usecols=2 )

            file_indx = 1
            for  line in rawF:
                file_indx += 1
                if file_indx % 2 == 0 :
                    dist_4_board_pause[arr_indx].append(line/1000)
                else:
                    dist_4_board_resume[arr_indx].append(line/1000)
        #inside first for loop
        arr_indx += 1

    # Done data should be separated into pause and resume arrays for the 3 lines
    # 1 board  [1,2,4,8,16]
    # 2 boards [  2,4,8,16]
    # 4 boards [    4,8,16]
        
    x_axis_vals_single_board = [1,2,4,8,16]
    y_axis_vals_pause_single_board = [avg(single_board_pause[0]),
                                      avg(single_board_pause[1]),
                                      avg(single_board_pause[2]),
                                      avg(single_board_pause[3]),
                                      avg(single_board_pause[4])]

    std_pause_single_board = [stdev(single_board_pause[0]),
                              stdev(single_board_pause[1]),
                              stdev(single_board_pause[2]),
                              stdev(single_board_pause[3]),
                              stdev(single_board_pause[4])]
    
    for x in std_pause_single_board:
        x = x * 1.96

    y_axis_vals_resume_single_board = [avg(single_board_resume[0]),
                                       avg(single_board_resume[1]),
                                       avg(single_board_resume[2]),
                                       avg(single_board_resume[3]),
                                       avg(single_board_resume[4])]

    std_resume_single_board = [stdev(single_board_resume[0]),
                               stdev(single_board_resume[1]),
                               stdev(single_board_resume[2]),
                               stdev(single_board_resume[3]),
                               stdev(single_board_resume[4])]

    for x in std_resume_single_board:
        x = x * 1.96

    # 2 board
    x_axis_vals_dist_2_board = [2,4,8,16]
    y_axis_vals_pause_dist_2_board = [avg(dist_2_board_pause[0]),
                                      avg(dist_2_board_pause[1]),
                                      avg(dist_2_board_pause[2]),
                                      avg(dist_2_board_pause[3])]

    std_pause_dist_2_board = [stdev(dist_2_board_pause[0]),
                              stdev(dist_2_board_pause[1]),
                              stdev(dist_2_board_pause[2]),
                              stdev(dist_2_board_pause[3])]

    for x in std_pause_dist_2_board:
        x = x * 1.96

    y_axis_vals_resume_dist_2_board = [avg(dist_2_board_resume[0]),
                                       avg(dist_2_board_resume[1]),
                                       avg(dist_2_board_resume[2]),
                                       avg(dist_2_board_resume[3])]

    std_resume_dist_2_board = [stdev(dist_2_board_resume[0]),
                               stdev(dist_2_board_resume[1]),
                               stdev(dist_2_board_resume[2]),
                               stdev(dist_2_board_resume[3])]

    for x in std_resume_dist_2_board:
        x = x * 1.96

    # 4 board

    x_axis_vals_dist_4_board = [4,8,16]
    y_axis_vals_pause_dist_4_board = [avg(dist_4_board_pause[0]),
                                      avg(dist_4_board_pause[1]),
                                      avg(dist_4_board_pause[2])]

    std_pause_dist_4_board = [stdev(dist_4_board_pause[0]),
                              stdev(dist_4_board_pause[1]),
                              stdev(dist_4_board_pause[2])]

    for x in std_pause_dist_4_board:
        x = x * 1.96

    y_axis_vals_resume_dist_4_board = [avg(dist_4_board_resume[0]),
                                       avg(dist_4_board_resume[1]),
                                       avg(dist_4_board_resume[2])]

    std_resume_dist_4_board = [stdev(dist_4_board_resume[0]),
                               stdev(dist_4_board_resume[1]),
                               stdev(dist_4_board_resume[2])]

    for x in std_resume_dist_4_board:
        x = x * 1.96


    # time to plot    
    colors=['b','r','g','c','m','y','k','coral','steelblue','tan']
    linestyles = ['-','--','-.',':','-','--']


    ax = plt.subplot(1,1,1)

    plt.errorbar(x_axis_vals_single_board,
                 y_axis_vals_pause_single_board,
                 fmt=colors[0],
                 capsize = 5,
                 yerr = std_pause_single_board,
                 label = 'Pause Operation on 1 Banana $\pi$',
                 linestyle = linestyles[0] )
    plt.errorbar(x_axis_vals_single_board,
                 y_axis_vals_resume_single_board,
                 fmt=colors[1],
                 capsize = 5,
                 yerr = std_resume_single_board,
                 label = 'Resume Operation on 1 Banana $\pi$',
                 linestyle = linestyles[1])

    plt.errorbar(x_axis_vals_dist_2_board,
                 y_axis_vals_pause_dist_2_board,
                 fmt=colors[2],
                 capsize = 5,
                 yerr = std_pause_dist_2_board,
                 label = 'Pause Operation on 2 Banana $\pi$s',
                 linestyle = linestyles[2])
    plt.errorbar(x_axis_vals_dist_2_board,
                 y_axis_vals_resume_dist_2_board,
                 fmt=colors[3],
                 capsize = 5,
                 yerr = std_resume_dist_2_board,
                 label = 'Resume Operation on 2 Banana $\pi$s',
                 linestyle = linestyles[3])

    plt.errorbar(x_axis_vals_dist_4_board,
                 y_axis_vals_pause_dist_4_board,
                 fmt=colors[4],
                 capsize = 5,
                 yerr = std_pause_dist_4_board,
                 label = 'Pause Operation on 4 Banana $\pi$s',
                 linestyle = linestyles[4])
    plt.errorbar(x_axis_vals_dist_4_board,
                 y_axis_vals_resume_dist_4_board,
                 fmt=colors[5],
                 capsize = 5,
                 yerr = std_resume_dist_4_board,
                 label = 'Resume Operation on 4 Banana $\pi$s',
                 linestyle = linestyles[5])

    #plt.axis([0,17,0,5])
    plt.legend(fontsize=12,loc='upper left')
    #ax.set_xscale("log")
    #ax.set_yscale("log")
    plt.xlabel('Number of Virtual Time Processes', fontsize=14)
    plt.ylabel('Microseconds', fontsize=14)
    plt.grid(True)
    plt.title('Overhead of Pause and Resume Operations')
    plt.show()
    
def mean(data):
    """Return the sample arithmetic mean of data."""
    n = len(data)
    if n < 1:
        raise ValueError('mean requires at least one data point')
    return sum(data)/float(n) # in Python 2 use sum(data)/float(n)

def _ss(data):
    """Return sum of square deviations of sequence data."""
    c = mean(data)
    ss = sum((x-c)**2 for x in data)
    return ss

def stdev(data, ddof=0):
    """Calculates the population standard deviation
    by default; specify ddof=1 to compute the sample
    standard deviation."""
    n = len(data)
    if n < 2:
        raise ValueError('variance requires at least two data points')
    ss = _ss(data)
    pvar = ss/(n-ddof)
    return pvar**0.5

def avg(p):
    sum = 0.0;
    for x in p:
        sum += x
    return float(sum/len(p))

def plot_err_cdf(data1, data2):
    """draw cdf to see the error between without/with freeze"""
    err_data = []
    for x, y in zip(data1, data2):
        err_data.append(abs(x-y))
    sorted_err_data = np.sort(err_data)
    cdf_err = np.arange(len(sorted_err_data)) / float(len(sorted_err_data))

    avg = np.mean(data1)
    variance = []
    for x in data1:
        variance.append(abs(x-avg))
    num_bins = 5000
    counts, bin_edges = np.histogram(variance, bins=num_bins)
    cdf_variance = np.cumsum(counts) / float(len(variance))

    xlim_min = min(min(err_data), min(variance))
    xlim_max = max(max(err_data), max(variance))
    plt.figure()
    # plt.xlim(0.99 * xlim_min, 1.02 * xlim_max)
    # plt.xlim(0, 5000)
    p1 = plt.plot(sorted_err_data, cdf_err, 'b', label=label_err)
    p2 = plt.plot(bin_edges[1:], cdf_variance, 'r', linestyle='--',
                  label='Variance of GTOD')

    plt.legend(fontsize=12) #(loc='lower right')
    plt.xlabel('Time Error (Milliseconds)', fontsize=20)
    plt.ylabel('Cumulative Distribution', fontsize=20)
    plt.grid(True)
    # plt.show()
    plt.savefig('err_%s_cdf.eps' % topic_name, format='eps')



def plot_compare_cdf(pd, pr):
    """draw cdf to compare without/with freeze elapsed time"""
    label1 = ['2Proc','4Proc','8Proc','16Proc']
    labell = -1
    nums_plt = 2
    for data1,data2 in zip(pd,pr):
        plt.subplot(2,3,nums_plt)
        nums_plt+=1
        #global labell,label1
        labell += 1
        num_bins = 100
        counts1, bin_edges1 = np.histogram(data1, bins=num_bins)
        cdf1 = np.cumsum(counts1) / float(len(data1))
        counts2, bin_edges2 = np.histogram(data2, bins=num_bins)
        cdf2 = np.cumsum(counts2) / float(len(data2))
        
        xlim_min = min(min(data1), min(data2))
        xlim_max = max(max(data1), max(data2))
        #plt.figure()
        # plt.xlim(0.98 * xlim_min, 1.02 * xlim_max)
        # plt.xlim(65000, 70000)
        plt.ylim(0, 1.05)
        l1_t=('Pause w/ %s' % label1[labell])
        p1 = plt.plot(bin_edges1[1:], cdf1, 'b', label=l1_t)
        l2_t = ('Resume w/ %s'%label1[labell])
        p2 = plt.plot(bin_edges2[1:], cdf2, 'r', linestyle='--', label=l2_t)
        plt.legend(loc='lower right', fontsize=12)
        plt.xlabel('Overhead (Milliseconds)', fontsize=12)
        plt.ylabel('Cumulative Distribution', fontsize=12)
        plt.grid(True)
    plt.show()
    #plt.savefig('cmp_%s_cdf.eps' % topic_name, format='eps')

def plot_variance_cdf(data):
    avg = np.mean(data)
    variance = []
    for x in data:
        variance.append(abs(x-avg))
    num_bins = 10
    counts, bin_edges = np.histogram(variance, bins=num_bins)
    cdf = np.cumsum(counts) / float(len(variance))

    xlim_min = min(variance)
    xlim_max = max(variance)
    plt.figure()
    plt.xlim(0.99 * xlim_min, 1.02 * xlim_max)
    p = plt.plot(bin_edges[1:], cdf, 'b', label='Variance of GTOD')
    plt.legend(loc='lower right', fontsize=12)
    plt.xlabel('Absolute Variance (Microseconds)', fontsize=20)
    plt.ylabel('Cumulative Distribution', fontsize=20)
    plt.grid(True)
    #plt.show()
    plt.savefig('var_%s_cdf.eps' % topic_name, format='eps')

'''
def main():
    """draw 2 cdf figures"""
    data1 = np.loadtxt(bsl_file)
    data2 = np.loadtxt(vir_file)
    font = {'size':16}
    matplotlib.rc('lines', lw=2)
    matplotlib.rc('font', **font)
    plot_compare_cdf(data1, data2)
    plot_err_cdf(data1, data2)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-b', '--bsl_file', action='store')
    parser.add_argument('-v', '--vir_file', action='store')
    parser.add_argument('-d', '--dilation', action='store', default=1, type=int)
    parser.add_argument('--label1', action='store')
    parser.add_argument('--label2', action='store')
    parser.add_argument('--label_err', action='store', default='Abs Error')
    parser.add_argument('--topic_name', action='store')

    results = parser.parse_args()
    bsl_file = results.bsl_file
    vir_file = results.vir_file
    dilation = results.dilation
    label1 = results.label1
    label2 = results.label2
    topic_name = results.topic_name
    label_err = results.label_err
'''

start()
