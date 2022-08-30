## Imports
from subprocess import Popen
import time
import sys

import generateGraphs
import getStats
import os
import subprocess

addresses    = 'addresses' # addresses of the nodes
topofilename = 'topology.txt'

numServers = 50
numFaulty = 16
connectivity = 49

payloadSize = 16 # in Bytes 
numBcasts = 1000; # total num of broadcast per node
sleepTime = 100000; # delay between 2 broadcasts in microsec
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 100; # higher bound broadcast id for stats
experimentTime = 240 # in seconds
writingIntervals = 100

resultsfilename = 'fig8.txt'


# generate local addesses (for local experiments, use another function for distributed experiments)
def genLocalAddresses(numNodes):
    # we erase the content of the file
    open(addresses,'w').close()
    # write all addresses
    f = open(addresses,'a')
    for i in range(numNodes):
        host = "127.0.0.1"
        port = 8760+i
        f.write("id:"+str(i)+" host:"+host+" port:"+str(port)+"\n")
    f.close()

# 1 - Dolev
# 2 - Bracha-Dolev
# 3 - Opt Bracha-Dolev
# 4 - Bracha-CPA 
# 5 - Opt Bracha-CPA 
choice = 3

MBD_1 = False
MBD_2 = False
MBD_3 = False
MBD_4 = False
MBD_5 = False
MBD_6 = False
MBD_7 = False
MBD_8 = False
MBD_9 = False
MBD_10 = False
MBD_11 = False
MBD_12 = False

firstWrite = True

def printGraph(G):
    s = ''
    for i in range(G.number_of_nodes()):
        for j in range(i, G.number_of_nodes()):
            if G.has_edge(i, j):
                s += str(i) + ' ' + str(j) + '\n';
    return s

for numRepeats in range(5):
    for numFaulty in [1,4,7,10]:
        for connectivity in [4, 10, 16, 20, 24, 28]:
            
            if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
                continue
            
            print(">>>>>>>>>>>>>>>>>>> Generate addresses")

            f = open(addresses, 'w')
            for i in range(numServers):
                f.write('id:'+str(i)+' host:127.0.0.1 port:'+str(8760+i)+'\n')
            f.close()

            print(">>>>>>>>>>>>>>>>>>> Generate graph")
            G = generateGraphs.generate_KRegularKConnectedRandomGraph(numServers,connectivity,numFaulty)

            #topofilename = 'topologyCPA.txt'



            f = open(topofilename, 'w')
            f.write(str(numServers)+' '+str(numFaulty)+'\n')
            f.write(printGraph(G))
            f.close()
            
            f = open(topofilename, 'r')
            t = f.readline().rstrip('\n').split(' ')
            numServers = int(t[0])
            numFaulty = int(t[1])
            f.close()

            genLocalAddresses(numServers)
            
            for MBD_4 in [True, False]: 
                
                print(numServers, numFaulty, connectivity, MBD_4, 'repeat =', numRepeats)

                print(">>>>>>>>>>>>>>>>>>> Starting processes")
                
                for i in range(numServers):
                    if os.path.exists('log/bytesCounts_'+str(i)):
                        os.remove('log/bytesCounts_'+str(i))
                    if os.path.exists('log/deliver_'+str(i)):
                        os.remove('log/deliver_'+str(i))
                    if os.path.exists('log/msgCounts_'+str(i)):
                        os.remove('log/msgCounts_'+str(i))
                
                list_proc = []
                for i in range(numServers):

                    s = Popen(["./server", str(i), topofilename, str(payloadSize), str(choice), str(numBcasts), str(sleepTime), str(minBidMeasure), str(maxBidMeasure), str(MBD_1), str(MBD_2), str(MBD_3), str(MBD_4), str(MBD_5), str(MBD_6), str(MBD_7), str(MBD_8), str(MBD_9), str(MBD_10), str(MBD_11), str(MBD_12)], stdout=subprocess.DEVNULL,
                    stderr=subprocess.STDOUT)
                    list_proc.append(s)
                    
                time.sleep(experimentTime) # sleeping time in seconds
                print(">>>>>>>>>>>>>>>>>>> kill all processes")
                for s in list_proc:
                    s.kill()
                
                print(">>>>>>>>>>>>>>>>>>> get latency")
                [a,b] = getStats.getLatencies(numServers)
                print(a,b)

                print(">>>>>>>>>>>>>>>>>>> get throughput")
                [a,b,c] = getStats.getThroughput(numServers)
                print(a,b,c)

                print(">>>>>>>>>>>>>>>>>>> get network consumption")
                [a,b,c] = getStats.getNetworkConsumption(numServers)
                print(a,b,c)

                if firstWrite:
                    f = open(resultsfilename, 'w')
                    f.write(str(numServers)+'\t'+str(numFaulty)+'\t'+str( connectivity)+'\t'+str(MBD_4)+'\t'+str(a)+'\t'+str(b)+'\t'+str(c)+'\n')
                    f.close()
                    firstWrite = False
                else:
                    f = open(resultsfilename, 'a')
                    f.write(str(numServers)+'\t'+str(numFaulty)+'\t'+str( connectivity)+'\t'+str(MBD_4)+'\t'+str(a)+'\t'+str(b)+'\t'+str(c)+'\n')
                    f.close()

    

