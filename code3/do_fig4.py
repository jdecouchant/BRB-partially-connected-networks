## Imports
from subprocess import Popen
import time
import sys
import getpass
import os

import generateGraphs
import getStats

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

def printGraph(G, topofilename, numServers, numFaulty):
    f = open(topofilename, 'w')
    f.write(str(numServers)+' '+str(numFaulty)+'\n')
    for i in range(G.number_of_nodes()):
        for j in range(i, G.number_of_nodes()):
            if G.has_edge(i, j):
                f.write(str(i) + ' ' + str(j) + '\n');
    f.close()

# to run remote experiments
remote = True

# Local information:
username  = getpass.getuser()
localhost = "127.0.0.1"
pem       = "bcast.pem" # to access local machine
curdir    = os.getcwd()

# List of triples: hostname/username/pem/dir -- add as many machines as you want there
hosts = [(localhost,username,pem,curdir)]
# List of nodes: id/hostname/username/pem -- this will be filled out when creating the nodes below
nodes = []
# The nodes are also stored in a file -- this gets created when creating the nodes
addressesfile = 'addresses' # addresses of the nodes

sshOpt1  = "StrictHostKeyChecking=no"
sshOpt2  = "ConnectTimeout=10"

topofilename  = 'topology.txt'

numServers = 10
numFaulty_arr = [1,4,7,10]
connectivity_arr = [4,10,16,20,24,28]

payloadSize = 16000 # in Bytes
numBcasts = 1000; # total num of broadcast per node
sleepTime = 1000000; # delay between 2 broadcasts in microsec
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 500; # higher bound broadcast id for stats
experimentTime = 60 # in seconds
writingIntervals = 100

resultsfilename = './experiments/fig4.txt'

# 1 - Dolev, 2 - Bracha-Dolev, 3 - Opt Bracha-Dolev
# 4 - Bracha-CPA, 5 - Opt Bracha-CPA 
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

for MBD_4 in [True, False]:

    for numFaulty in numFaulty_arr:
        for connectivity in connectivity_arr:

            print('Running with N='+str(numServers)+', f='+str(numFaulty)+', connectivity ='+str(connectivity))
            if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
                print('\t Skip: connectivity too low compared to numFaulty')
                continue
            if numServers < 3*numFaulty + 1:
                print('\t Skip: not enough replicas')
                continue
            if connectivity >= numServers:
                print('\t Skip: connectivity too high compared to numServers')
                continue
            
            print(numServers, numFaulty, connectivity, MBD_4)
            print(">>>>>>>>>>>>>>>>>>> Generate addresses")

            #genLocalAddresses(numServers)

            f = open(addressesfile, 'w')
            for i in range(numServers):
                host      = localhost
                remotedir = curdir
                if remote:
                    # we cycle through the hosts
                    (host,username,pem,remotedir) = hosts[0]
                    hosts = hosts[1:]
                    hosts.append((host,username,pem,remotedir))
                f.write('id:'+str(i)+' host:' + host + ' port:'+str(8760+i)+'\n')
                nodes.append((i,host,username,pem,remotedir))
            f.close()

            print(">>>>>>>>>>>>>>>>>>> Generate graph")
            G = generateGraphs.generate_KRegularKConnectedRandomGraph(numServers,connectivity,numFaulty)

            printGraph(G, topofilename, numServers, numFaulty)

            f = open(topofilename, 'r')
            t = f.readline().rstrip('\n').split(' ')
            numServers = int(t[0])
            numFaulty = int(t[1])
            f.close()

            print(">>>>>>>>>>>>>>>>>>> Starting processes")
            list_proc = []
            for (i,host,username,pem,remotedir) in nodes:
                # ./server nodeId topologyFile
                #if i==0:
                    #s = Popen(["valgrind", "--tool=callgrind", "./server", str(i), topofilename, str(payloadSize), str(choice)])
                #else:
                if remote:
                    cmd    = "\"\"cd " + remotedir + " && ./server " + str(i) + " " + topofilename + " " + str(payloadSize) + " " + str(choice) + " " + str(numBcasts) + " " + str(sleepTime) + " " + str(minBidMeasure) + " " + str(maxBidMeasure) + " " + str(MBD_1) + " " + str(MBD_2) + " " + str(MBD_3) + " " + str(MBD_4) + " " + str(MBD_5) + " " + str(MBD_6) + " " + str(MBD_7) + " " + str(MBD_8) + " " + str(MBD_9) + " " + str(MBD_10) + " " + str(MBD_11) + " " + str(MBD_12) + " " + str(writingIntervals) + "\"\""
                    ## TODO: we might want to copy the files there if they are not already there
                    sshAdr = username + "@" + host
                    if pem == "":
                        s = Popen(["ssh","-o",sshOpt1,"-ntt",sshAdr,cmd])
                    else:
                        s = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
                else:
                    s = Popen(["./server", str(i), topofilename, str(payloadSize), str(choice), str(numBcasts), str(sleepTime), str(minBidMeasure), str(maxBidMeasure), str(MBD_1), str(MBD_2), str(MBD_3), str(MBD_4), str(MBD_5), str(MBD_6), str(MBD_7), str(MBD_8), str(MBD_9), str(MBD_10), str(MBD_11), str(MBD_12), str(writingIntervals)])
                list_proc.append(s)

            time.sleep(experimentTime) # sleeping time in seconds
            print(">>>>>>>>>>>>>>>>>>> kill all processes")
            for s in list_proc:
                s.kill()

            print(">>>>>>>>>>>>>>>>>>> get latency")
            [a,b,c,d,e] = getStats.getLatencies(numServers)
            print(a,b,c,d,e) # average, minLat, maxLat, stddev, countAvg

            # write results to .txt file for later plot
            latFile = open('experiments/lat_fig4.txt','a')
            latFile.write(str(MBD_4)+'\t'+str(numServers)+'\t'+str(numFaulty)+'\t'+str(connectivity)+\
                          '\t'+str(a)+'\t'+str(b)+'\t'+str(c)+'\t'+str(d)+'\t'+str(e)+'\n')
            latFile.close()

            print(">>>>>>>>>>>>>>>>>>> get throughput")
            [a,b,c] = getStats.getThroughput(numServers)
            print(a,b,c) # average, min, max

            print(">>>>>>>>>>>>>>>>>>> get network consumption")
            [a,b,c] = getStats.getNetworkConsumption(numServers)
            print(a,b,c) # average, min, max



