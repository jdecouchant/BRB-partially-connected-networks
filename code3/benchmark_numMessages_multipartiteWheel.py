## Imports
import time
import sys
import getpass
import os
import argparse
import subprocess
import shutil

import generateGraphs
import getStats

## Parameters

# to run remote experiments
remote = False

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

numServers = 31

payloadSize = 16 # in Bytes 
numBcasts = 5 * numServers; # total num of broadcast per node
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 100; # higher bound broadcast id for stats
writingIntervals = 1

resultsfilename = 'numMsgs_randomGraphs_multipartiteWheel.txt'

experimentTime = {1:120, 4:120, 7:120, 10:120} # in sec
sleepTime = {1:2000000, 4:2000000, 7:2000000, 10:2000000} # in microsec

# generate local addesses (for local experiments, use another function for distributed experiments)
def genLocalAddresses(numNodes):
    # we erase the content of the file
    open(addressesfile,'w').close()
    # write all addresses
    f = open(addressesfile,'a')
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

#MBD_1 = True # Associate payload to ID to avoid resending + delete echo or ready sender with single hop
#MBD_2 = True # Single-hop Send msgs // PB with this option
#MBD_3 = True # Echo_Echo messages
#MBD_4 = True # Ready_Echo messages
#MBD_5 = True # Optimized Send messages 
#MBD_6 = True # Ignore Echos rcvd after corresponding Ready delivered 
#MBD_7 = True # Ignore all Echos rcvd after delivering content 
#MBD_8 = True # Do not send Echos to Ready neighbor
#MBD_9 = True # Do not send anything to neighbor that delivered 
#MBD_10 = True # Ignore msgs whose path is a superpath of known path 
#MBD_11 = True # Bracha overprovisioning - to improve in send_echo and send_ready 
#MBD_12 = True # Source sends its new Echo to only 2f+1 other processes

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

# time ok for numfaulty = 1

s = subprocess.Popen("make", shell=True)
#s = subprocess.Popen("make clean; make", shell=True)
s.wait()

def stopAll():
    for (host,username,pem,remotedir) in hosts:
        if remote:
            cmd    = "\"\"killall server\"\""
            sshAdr = username + "@" + host
            if pem == "":
                s = subprocess.Popen(["ssh","-o",sshOpt1,"-ntt",sshAdr,cmd])
            else:
                s = subprocess.Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        else:
            s = subprocess.Popen(["killall", "server"])

for numRepeats in range(5):
    for connectivity in [4, 8, 12, 16, 20, 24, 28]:
        for numFaulty in [1,4,7,10]: #10,7,4,1]:       
            
            if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
                continue
            
            print(">>>>>>>>>>>>>>>>>>> Generate addresses")
            #genLocalAddresses(numServers)
            f = open(addressesfile, 'w')
            username  = getpass.getuser()
            pem       = "bcast.pem"
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
            #G = generateGraphs.generate_KRegularKConnectedRandomGraph(numServers,connectivity,numFaulty)
            
            G = generate_generalizedWheel(numServers, connectivity)

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
            
            for MBD_11 in [False, True]: 
                
                print(numServers, numFaulty, connectivity, MBD_11, 'repeat =', numRepeats)

                print(">>>>>>>>>>>>>>>>>>> Starting processes")
                
                for i in range(numServers):
                    if os.path.exists('log/bytesCounts_'+str(i)):
                        os.remove('log/bytesCounts_'+str(i))
                    if os.path.exists('log/deliver_'+str(i)):
                        os.remove('log/deliver_'+str(i))
                    if os.path.exists('log/msgCounts_'+str(i)):
                        os.remove('log/msgCounts_'+str(i))
                    
                print(">>>>>>>>>>>>>>>>>>> Starting processes")
                list_proc = []
                for (i,host,username,pem,remotedir) in nodes:
                    # ./server nodeId topologyFile
                    #if i==0:
                    #s = Popen(["valgrind", "--tool=callgrind", "./server", str(i), topofilename, str(payloadSize), str(choice)])
                    #else:
                    if remote:
                        cmd    = "\"\"cd " + remotedir + " && ./server " + str(i) + " " + topofilename + " " + str(payloadSize) + " " + str(choice) + " " + str(numBcasts) + " " + str(sleepTime[numFaulty]) + " " + str(minBidMeasure) + " " + str(maxBidMeasure) + " " + str(MBD_1) + " " + str(MBD_2) + " " + str(MBD_3) + " " + str(MBD_4) + " " + str(MBD_5) + " " + str(MBD_6) + " " + str(MBD_7) + " " + str(MBD_8) + " " + str(MBD_9) + " " + str(MBD_10) + " " + str(MBD_11) + " " + str(MBD_12) + " " + str(writingIntervals) + "\"\""
                        ## TODO: we might want to copy the files there if they are not already there
                        sshAdr = username + "@" + host
                        if pem == "":
                            s = subprocess.Popen(["ssh","-o",sshOpt1,"-ntt",sshAdr,cmd])
                        else:
                            s = subprocess.Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
                    else:
                        #print(str(i), topofilename, str(payloadSize), str(choice), str(numBcasts), str(sleepTime), str(minBidMeasure), str(maxBidMeasure), str(MBD_1), str(MBD_2), str(MBD_3), str(MBD_4), str(MBD_5), str(MBD_6), str(MBD_7), str(MBD_8), str(MBD_9), str(MBD_10), str(MBD_11), str(MBD_12), str(writingIntervals))
                        s = subprocess.Popen(["./server", str(i), topofilename, str(payloadSize), str(choice), str(numBcasts), str(sleepTime[numFaulty]), str(minBidMeasure), str(maxBidMeasure), str(MBD_1), str(MBD_2), str(MBD_3), str(MBD_4), str(MBD_5), str(MBD_6), str(MBD_7), str(MBD_8), str(MBD_9), str(MBD_10), str(MBD_11), str(MBD_12), str(writingIntervals)])
                    list_proc.append(s)
                    
                time.sleep(experimentTime[numFaulty]) # sleeping time in seconds
                print(">>>>>>>>>>>>>>>>>>> kill all processes")
                for s in list_proc:
                    s.kill()
                    
                for i in range(numServers):
                    subprocess.Popen(["./killByPort.sh", str(8760+i)])
                #time.sleep(10)

                numEffectiveBcasts = getStats.getNumEffectiveBroadcasts()

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
                [a,b,c] = getStats.getNetworkConsumption(numServers)
                print(a,b,c)
                
                print(">>>>>>>>>>>>>>>>>>> get num messages")
                [a,b,c,d] = getStats.getNumMsgs(numServers)
                print(a,b,c,d)

                if firstWrite:
                    f = open(resultsfilename, 'w')
                    f.write(str(numServers)+'\t'+str(numFaulty)+'\t'+str( connectivity)+'\t'+str(MBD_11)+'\t'+str(a)+'\t'+str(b)+'\t'+str(c)+'\t'+str(d)+'\n')
                    f.close()
                    firstWrite = False
                else:
                    f = open(resultsfilename, 'a')
                    f.write(str(numServers)+'\t'+str(numFaulty)+'\t'+str( connectivity)+'\t'+str(MBD_11)+'\t'+str(a)+'\t'+str(b)+'\t'+str(c)+'\t'+str(d)+'\n')
                    f.close()
                    
                #dirname = "log/"+str(numFaulty)+"_"+str(numServers)+"_"+str(connectivity)+"_"+str(numRepeats)
                
                #if os.path.exists(dirname) and os.path.isdir(dirname):
                    #shutil.rmtree(dirname)
                
                #os.mkdir(dirname)
                    
                #for i in range(numServers):
                    #f1 = 'bytesCounts_'+str(i)
                    #if os.path.exists('log/'+f1):
                        #os.rename('log/'+f1, dirname+'/'+f1)
                    #f2 = 'deliver_'+str(i)
                    #if os.path.exists('log/'+f2):
                        #os.rename('log/'+f2, dirname+'/'+f2)
                    #f3 = 'msgCounts_'+str(i)
                    #if os.path.exists('log/'+f3):
                        #os.rename('log/'+f3, dirname+'/'+f3)
                


