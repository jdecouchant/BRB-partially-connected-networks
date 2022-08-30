from subprocess import Popen
import time
import sys
import os

import generateGraphs

import numpy as np
import matplotlib as mp
import matplotlib.pyplot as plt

import statistics

x1 = [1] 
y1 = [20]

nMsgs = {}

numServers = 31
for numFaulty in [1,4,7,10]:
    for connectivity in [4, 8, 12, 16, 20, 24, 28]:

        if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
            continue
        if not numFaulty in nMsgs:
            nMsgs[numFaulty] = {}
        if not connectivity in nMsgs[numFaulty]:
            nMsgs[numFaulty][connectivity] = {}
        
        nMsgs[numFaulty][connectivity][False] = []
        nMsgs[numFaulty][connectivity][True] = []

fname = 'fig6.txt'
f = open(fname, 'r')

for l in f:
    l = l.split('\t')

    if l[0]=='':
        continue

    numServers = int(l[0])
    numFaulty = int(l[1])
    connectivity = int(l[2])

    MBD11 = (l[3] == 'True')

    x = float(l[4]) # average

    print(numFaulty, numServers, connectivity)
    nMsgs[numFaulty][connectivity][MBD11].append(x)

f.close()

x = {}
y = {}
nonAvgY = {}

def Average(lst):
    return sum(lst) / len(lst)

for numFaulty in [1,4,7,10]:
    x[numFaulty] = []
    y[numFaulty] = []
    nonAvgY[numFaulty] = []
    for connectivity in [4, 8, 12, 16, 20, 24, 28]:

        if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
            continue

##        print(numFaulty, connectivity, lat[numFaulty][connectivity])

        if len(nMsgs[numFaulty][connectivity][False]) == 0 or \
           len(nMsgs[numFaulty][connectivity][True]) == 0:
            continue


        x[numFaulty].append(connectivity)
        
        wo = Average(nMsgs[numFaulty][connectivity][False])
        w = Average(nMsgs[numFaulty][connectivity][True])
        y[numFaulty].append(100*(w-wo)/wo)

        for k in range(len(nMsgs[numFaulty][connectivity][True])):
            wo = nMsgs[numFaulty][connectivity][False][k]
            w = nMsgs[numFaulty][connectivity][True][k]
            nonAvgY[numFaulty].append(100*(w-wo)/wo)

#yerr = [[(avgThroughput-minThroughput), (maxThroughput-avgThroughput)]] #np.random.random_sample(1)
#yerr = np.transpose(yerr)

fig, ax = plt.subplots()

print(nonAvgY)

SMALL_SIZE = 8
MEDIUM_SIZE = 10
BIGGER_SIZE = 12

plt.rc('font', size=BIGGER_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=BIGGER_SIZE)     # fontsize of the axes title
plt.rc('axes', labelsize=BIGGER_SIZE)    # fontsize of the x and y labels
##plt.rc('xtick', labelsize='x-large')    # fontsize of the tick labels
##plt.rc('ytick', labelsize=BIGGER_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=BIGGER_SIZE)    # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=12)
ax.tick_params(axis='both', which='minor', labelsize=12)

ax.errorbar(x[1], y[1], yerr=np.std(nonAvgY[1]), marker='o', linestyle='solid', label='N=31, f=1')
ax.errorbar(x[4], y[4], yerr=np.std(nonAvgY[4]), marker='x', linestyle='dotted', label='N=31, f=4')
ax.errorbar(x[7], y[7], yerr=np.std(nonAvgY[7]), marker='^', linestyle='dashed', label='N=31, f=7')
ax.errorbar(x[10], y[10], yerr=np.std(nonAvgY[10]), marker='+', linestyle='dashdot', label='N=31, f=10')

ax.legend()


ax.set_xlabel('Network connectivity (k)', fontsize=BIGGER_SIZE)
ax.set_ylabel('Variation #bits transmitted (%)', fontsize=BIGGER_SIZE)
#ax.set_title('Line plot with error bars')

##plt.ylim([-100, 0])
plt.xlim([0, 32])
plt.xticks([0,4,8,12,16,20,24,28,32])

plt.show()


