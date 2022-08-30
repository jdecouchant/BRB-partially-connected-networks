## Imports
import subprocess
from subprocess import Popen
import time
import sys
import getpass
import os
import argparse

import re
import generateGraphs
import getStats
import json
import math

## Parameters

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

# Docker parameters 

docker = "docker"
dockerBase = "brachadolev" # name of the container image

# Cluster parameters
clusterNet = "brachaDolev"

ipsOfNodes   = {} # dictionnary mapping node ids to IPs

numServers = 30
numFaulty = 1
connectivity = 3

payloadSize = 1024 # in Bytes
numBcasts = 100 # total num of broadcast per node
sleepTime = 5000000; # delay between 2 broadcasts in microsec
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 500; # higher bound broadcast id for stats
experimentTime = 60 # in seconds
writingIntervals = 1
firstPort = 10000

# 1 - Dolev, 2 - Bracha-Dolev, 3 - Opt Bracha-Dolev, 4 - Bracha-CPA, 5 - Opt Bracha-CPA
choice = 3

MBD_1 = False # Give ID to payload to avoid resending + delete echo or ready sender with single hop
MBD_2 = False # Single-hop Send msgs // PB with this option
MBD_3 = False # Echo_Echo messages
MBD_4 = False # Ready_Echo messages
MBD_5 = False # Optimized Send messages 
MBD_6 = False # Ignore Echos rcvd after corresponding Ready delivered 
MBD_7 = False # Ignore all Echos rcvd after delivering content 
MBD_8 = False # Do not send Echos to Ready neighbor
MBD_9 = False # Do not send anything to neighbor that delivered 
MBD_10 = False # Ignore msgs whose path is a superpath of known path 
MBD_11 = False # Bracha overprovisioning - to improve in send_echo and send_ready 
MBD_12 = False # Source sends its new Echo to only 2f+1 other processes

if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
    print('Incorrect connectivity')
    sys.exit()

# generate local addesses (for local experiments, use another function for distributed experiments)
def genLocalAddresses(numNodes):
    # write all addresses
    f = open(addressesfile,'w')
    for i in range(numNodes):
        host = ipsOfNodes[i] 
        port = firstPort+i
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

def runExperiment():
    global numServers
    global numFaulty
    global hosts

    print(numServers, numFaulty, connectivity, MBD_4)
        
    numEffectiveBcasts = getStats.getNumEffectiveBroadcasts(numServers)
    print('Num effective bcasts:', numEffectiveBcasts)
    
    print(">>>>>>>>>>>>>>>>>>> get num delivers")
    numDelivers = getStats.getNumDelivers(numServers)
    for x in numDelivers: 
        print(str(x/numEffectiveBcasts*100)+'%', end=' ')
    print('')

    print(">>>>>>>>>>>>>>>>>>> get latency")
    [a,b,c,d,e] = getStats.getLatencies(numServers)
    print(a,b,c,d,e)

    print(">>>>>>>>>>>>>>>>>>> get throughput")
    [a,b,c] = getStats.getThroughput(numServers)
    print(a,b,c)

    print(">>>>>>>>>>>>>>>>>>> get network consumption")
    [a,b,c,d] = getStats.getNetworkConsumption(numServers)
    print(a,b,c,d)

parser = argparse.ArgumentParser(description='Bracha-Dolev')
parser.add_argument("--start", action="store_true")
parser.add_argument("--stop",  action="store_true")
args = parser.parse_args()


if args.start:
    print("lauching experiment")
    runExperiment()
elif args.stop:
    print("stoping experiment")
    stopAll()
else:
    print("lauching experiment")
    runExperiment()
