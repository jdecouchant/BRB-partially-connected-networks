## Imports
from subprocess import Popen
import time
import sys
import os
#import numpy as np

import generateGraphs

#import matplotlib.pyplot as plt

import statistics

def getLatencies(numServers):
    
    broadcastTimes = {}
    for i in range(numServers):
        fname = 'log/broadcastTimes_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            for l in f: 
                l = l.rstrip('\n').split('\t')
                broadcasterId = int(l[0])
                broadcast = int(l[1])
                time = int(l[2])
                if not broadcasterId in broadcastTimes:
                    broadcastTimes[broadcasterId] = {}            
                broadcastTimes[broadcasterId][broadcast] = time
            f.close()   
    
    deliveryTimes = {}
    for i in range(numServers): 
        #print('Opening file ', i)
        fname = 'log/deliver_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            for l in f:
                try:
                    l = l.rstrip('\n').split('\t')
                    nodeId = int(l[0])
                    broadcast = int(l[1])
                    time = int(l[2])
                    
                    if not nodeId in deliveryTimes:
                        deliveryTimes[nodeId] = {}
                    if not broadcast in deliveryTimes[nodeId]:
                        deliveryTimes[nodeId][broadcast] = []
                    deliveryTimes[nodeId][broadcast].append(time)
                except Exception:
                    print('Exception happened!')
                    continue
            f.close()
    #print(deliveryTimes)

    countAvg = 0
    lat_arr = []

    for nodeId in deliveryTimes: 
        for broadcastId in deliveryTimes[nodeId]:
            #if (nodeId==0): 
                #print(nodeId, broadcastId, len(deliveryTimes[nodeId][broadcastId]))
            
            if len(deliveryTimes[nodeId][broadcastId]) >= int(0.9 * numServers):
                sendTime = broadcastTimes[nodeId][broadcastId]
                for i in range(int(0.9 * numServers)):
                    
                    if deliveryTimes[nodeId][broadcastId][i]-sendTime >= 0: 
                        lat_arr.append(deliveryTimes[nodeId][broadcastId][i]-sendTime)
                    else: 
                        print("Negative latency with", nodeId)
                    #print("("+str(i)+","+str(deliveryTimes[nodeId][broadcastId][i]-sendTime)+")", end=' ')
                    #avg += (deliveryTimes[nodeId][broadcastId][i]-sendTime)
                #print()
                countAvg += 1
                
    #print('Avg latency (us) = ', str(allAvg/countAvg)+', stddev =', statistics.pstdev(lat_arr) )
    if len(lat_arr) != 0:
        return [int(sum(lat_arr)/len(lat_arr)), min(lat_arr), max(lat_arr), 0, countAvg]
        #return [sum(lat_arr)/len(lat_arr), min(lat_arr), max(lat_arr), np.std(lat_arr), countAvg]
    else:
        return [-1, -1, -1, -1, -1]

    #x = [1] 
    #y = [allAvg/countAvg / 1000000]
    #yerr = [[(allAvg/countAvg-minLat) / 1000000, (maxLat-allAvg/countAvg)/ 1000000]] #np.random.random_sample(1)
    #yerr = np.transpose(yerr)

    #fig, ax = plt.subplots()

    #ax.errorbar(x, y,
                #yerr=yerr,
                #fmt='-o')

    #ax.set_xlabel('x-axis')
    #ax.set_ylabel('Latency (s)')
    ##ax.set_title('Line plot with error bars')

    #plt.ylim([0, 50])

    ##plt.show()

def getThroughput(numServers):

    avgThroughput = 0
    minThroughput = 1000000000
    maxThroughput = 0

    thr_arr  = [1] 
    #y = [allAvg/countAvg / 1000000]
    #yerr = [[(allAvg/countAvg-minLat) / 1000000, (maxLat-allAvg= []

    for i in range(numServers): 
        countDeliveries = 0
        minTime = 0
        maxTime = 0
        
        fname = 'log/deliver_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            for l in f: 
                l = l.rstrip('\n').split('\t')
                broadcasterId = l[0]
                broadcast = l[1]
                time = int(l[2])
                
                if minTime == 0:
                    minTime = time
                    
                maxTime = time
                
                countDeliveries += 1
            f.close()
        
        if minTime != maxTime: 
            throughput = countDeliveries / ((maxTime - minTime)/1000000)
        
            thr_arr.append(throughput)
        
            avgThroughput += throughput
            minThroughput = min(minThroughput, throughput)
            maxThroughput = max(maxThroughput, throughput)
        
    
    avgThroughput /= numServers
    #print('Avg throughput (req/s) =', avgThroughput) 
    #for i in range(numServers): 
        #print('\t['+str(i)+']', thr_arr[i])
    return [int(avgThroughput), minThroughput, maxThroughput]

#x = [1] 
#y = [avgThroughput]
#yerr = [[(avgThroughput-minThroughput), (maxThroughput-avgThroughput)]] #np.random.random_sample(1)
#yerr = np.transpose(yerr)

#fig, ax = plt.subplots()

#ax.errorbar(x, y,
            #yerr=yerr,
            #fmt='-o')

#ax.set_xlabel('x-axis')
#ax.set_ylabel('Throughput (req/s)')
##ax.set_title('Line plot with error bars')

#plt.ylim([0, 50])

##plt.show()


def getNetworkConsumption(numServers):
    net = []
    
    for i in range(numServers): 
        fname = 'log/bytesCounts_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            for l in f: 
                if '#' == l[0]:
                    continue
                
                l = l.rstrip('\n').split('\t')
                total = int(l[19])
                
                net.append(total)
                
            f.close()
        
    if len(net) != 0:
        #return [sum(net)/len(net), min(net), max(net), np.std(net)]
        return [int(sum(net)/len(net)), min(net), max(net), 0]
    else:
        return [-1, -1, -1, -1]
    
def getNumEffectiveBroadcasts(numServers): 
    count = 0
    for i in range(numServers):
        fname = 'log/broadcastTimes_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            for l in f: 
                count += 1
            f.close()   
    return count
    
def getNumDelivers(numServers): 
    numDelivers = []
    
    for i in range(numServers):
        fname = 'log/deliver_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            count = 0
            for l in f: 
                count += 1
            f.close()   
            numDelivers.append(count)
        else: 
            numDelivers.append(0)
            
    return numDelivers

def getNumMsgs(numServers):
    
    msgs = []
    for i in range(numServers): 
        
        fname = 'log/msgCounts_'+str(i)
        if os.path.exists(fname):
            f = open(fname, 'r')
            for l in f: 
                if '#' == l[0]:
                    continue
                
                l = l.rstrip('\n').split('\t')

                total = int(l[-1])
                msgs.append(total)
                
            f.close()
        
    if len(msgs) != 0:
        #return [sum(msgs)/len(msgs), min(msgs), max(msgs), np.std(msgs)]
        return [sum(msgs)/len(msgs), min(msgs), max(msgs), 0]
    else:
        return [-1, -1, -1, -1]

#x = [1] 
#y = [avgBdw]
#yerr = [[(avgBdw-minBdw), (maxBdw-avgBdw)]] #np.random.random_sample(1)
#yerr = np.transpose(yerr)

#fig, ax = plt.subplots()

#ax.errorbar(x, y,
            #yerr=yerr,
            #fmt='-o')

#ax.set_xlabel('x-axis')
#ax.set_ylabel('Bandwidth consumption (Bytes)')
##ax.set_title('Line plot with error bars')

##plt.ylim([0, 50])

##plt.show()








#avgMsgs = 0
#minMsgs = 1000000000
#maxMsgs = 0
#countMsgs = 0

#msg_arr = []

#for i in range(numServers): 
    
    #localAvg = 0
    #localCount = 0
    
    #fname = 'log/msgCounts_'+str(i)
    #f = open(fname, 'r')
    #for l in f: 
        #if '#' == l[0]:
            #continue
        
        #l = l.rstrip('\n').split('\t')
        #total = int(l[19])
        
        #minMsgs = min(minMsgs, total)
        #maxMsgs = max(maxMsgs, total)
        
        #avgMsgs += total
        #localAvg += total
        #localCount += 1
        #countMsgs += 1
        
    #msg_arr.append(localAvg/localCount)
        
    #f.close()
    
#avgMsgs /= countMsgs

#print('Avg num msgs =', avgMsgs, '['+str(minMsgs)+", "+str(maxMsgs)+"]")
#for i in range(len(msg_arr)):
    #print('\t['+str(i)+']', msg_arr[i])

#x = [1] 
#y = [avgMsgs]
#yerr = [[(avgMsgs-minMsgs), (maxMsgs-avgMsgs)]] #np.random.random_sample(1)
#yerr = np.transpose(yerr)

#fig, ax = plt.subplots()

#ax.errorbar(x, y,
            #yerr=yerr,
            #fmt='-o')

#ax.set_xlabel('x-axis')
#ax.set_ylabel('Number of messages')
##ax.set_title('Line plot with error bars')

##plt.ylim([0, 50])

##plt.show()
            
