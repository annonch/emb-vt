import numpy as np

data = np.loadtxt("vt_log.txt", delimiter = ',', usecols = [1,3])

#d = data[:,1] - 2*data[:,0]
d = np.transpose(np.vstack((np.diff(data[:,0]),np.diff(data[:,1]))))
#d = data[:,1] / data[:,0]
np.savetxt("deltas3.csv", d, delimiter = ',')