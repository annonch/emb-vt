#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import argparse

#matplotlib.rcParams['ps.useafm'] = True
#matplotlib.rcParams['pdf.use14corefonts'] = True
#matplotlib.rcParams['text.usetex'] = True







def start(case1 = True):
    runs = [4,8,16]
    nums = -1
    #files = ['skew_2.log','skew_4.log','skew_8.log'] #,'skew_16.log']
    files = ['merged_2.log','merged_4.log','merged_8.log'] #,'skew_16.log']

    for file in files:
        nums+=1
        rawF = np.genfromtxt(file, delimiter=',')#, usecols={0,1} )
        print(rawF)
        print len(rawF)
        
        for y in reversed(range(runs[nums])):
            print rawF[:,y]
            if case1:
                rawF[:,y] = (rawF[:,y] - rawF[:,0])/1000000
            else:
                rawF[:,y] = (rawF[:,y] - rawF[:,0])/1000
            #print rawF[:,y]
        #print(rawF)
        #print len((rawF[:,1]))
        #print len(range(len(rawF[:,0])))

        #rawFT = rawF.transpose()
        colors=['b','r','g','c','m','y','k','coral','steelblue','tan','olive','orchid',
                'darksalmon', 'lightslategrey','steelblue']

        plt.subplot(2,2,nums+1)

        for x in range(1,runs[nums]):
            l="process %s"%(x+1)
            if case1:
                plt.plot(range(len(rawF[:,0])),rawF[:,x],c=colors[x-1],label=l)
            else:
                plt.plot(range(len(rawF[:,0])),rawF[:,x]/range(len(rawF[:,0])),c=colors[x-1],label=l)
            
        #plt.axis([-1,100,0,15])
        #plt.axis([-1,100,0,15])
        
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
'''    









        
        pause.append([])
        resume.append([])

        #print rawF
        #pause = 0,2,4, ...
        
        indx = 1
        for  el in rawF:
            indx += 1
            if indx % 2 == 0 :
                pause[rank].append(el/1000000)
            else:
                resume[rank].append(el/1000000)
        rank += 1
    
    #pause
    #  1H | 2H | 4H | 8H | 16H 
    x_axis_vals = [1,2,4,8,16]
    y_axis_vals_pause = [avg(pause[0]), avg(pause[1]), avg(pause[2]), avg(pause[3]), avg(pause[4])]

    std_pause = [stdev(pause[0]),stdev(pause[1]),stdev(pause[2]),stdev(pause[3]),stdev(pause[4])]
    for x in std_pause:
        x = x* 1.96

    y_axis_vals_resume = [avg(resume[0]), avg(resume[1]), avg(resume[2]), avg(resume[3]), avg(resume[4])]
    std_resume = [stdev(resume[0]),stdev(resume[1]),stdev(resume[2]),stdev(resume[3]),stdev(resume[4])]

    for x in std_resume:
        x = x* 1.96
    #print y_axis_vals_resume
    #print x_axis_vals
    #print y_axis_vals_pause
    #print std_pause

    plt.errorbar(x_axis_vals, y_axis_vals_pause,fmt='b', capsize = 5, yerr = std_pause, label = 'pause', linestyle = '--' )
    plt.errorbar(x_axis_vals, y_axis_vals_resume,fmt='r',capsize = 5, yerr = std_resume, label = 'resume')
    plt.axis([0,17,0,15])
    plt.legend(fontsize=12,loc='lower right')

    plt.xlabel('Number of Virtual Time Processes', fontsize=14)
    plt.ylabel('Milliseconds', fontsize=14)
    plt.grid(True)
    plt.title('Overhead of Pause and Resume')
    plt.show()

    ########
    # cdfs #
    ########
    
    plot_compare_cdf(pause,resume)
    


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

colors=['b','r','g','c','m','y','k','coral','steelblue','tan']

linestyles = ['solid','dotted']

def plot_compare_cdf(pd, pr):
    """draw cdf to compare without/with freeze elapsed time"""
    label1 = ['1Host','2Host','4Host','8Host','16Host']
    labell = -1

    for data1,data2 in zip(pd,pr):
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

    start()


'''
