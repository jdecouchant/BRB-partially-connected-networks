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

# Docker parameters 

docker = "docker"
dockerBase = "brachadolev" # name of the container image

# Cluster parameters
clusterNet = "brachaDolev"

ipsOfNodes   = {} # dictionnary mapping node ids to IPs

numServers = 4
numFaulty = 1
connectivity = 3

payloadSize = 10 # in Bytes
numBcasts = 100 # total num of broadcast per node
sleepTime = 5000000; # delay between 2 broadcasts in microsec
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 500; # higher bound broadcast id for stats
experimentTime = 240 # in seconds
writingIntervals = 1

resultsfilename = 'fig4.txt'

# 1 - Dolev, 2 - Bracha-Dolev, 3 - Opt Bracha-Dolev, 4 - Bracha-CPA, 5 - Opt Bracha-CPA
choice = 3

doPrintGraph = True

MBD_1 = False # Associate payload to ID to avoid resending + delete echo or ready sender with single hop
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


#print(">>>>>>>>>>>>>>>>>>> get latency")
#[a,b] = getStats.getLatencies(numServers)
#print(a,b)
#sys.exit()


def runExperiment():
    global numServers
    global numFaulty
    global hosts

    print(numServers, numFaulty, connectivity, MBD_4)
    print(">>>>>>>>>>>>>>>>>>> Generate addresses")

    #genLocalAddresses(numServers)

    #f = open(addressesfile, 'w')
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
        #f.write('id:'+str(i)+' host:172.17.0.' + str(i+2) + ' port:'+str(8760+i)+'\n')
        nodes.append((i,host,username,pem,remotedir))
    #f.close()

    print(">>>>>>>>>>>>>>>>>>> Generate graph")
    G = generateGraphs.generate_KRegularKConnectedRandomGraph(numServers,connectivity,numFaulty)

    if doPrintGraph:
        printGraph(G, topofilename, numServers, numFaulty)

    f = open(topofilename, 'r')
    t = f.readline().rstrip('\n').split(' ')
    numServers = int(t[0])
    numFaulty = int(t[1])
    f.close()
    
    sshOpt1  = "StrictHostKeyChecking=no"
    sshOpt2  = "ConnectTimeout=10"
    
    s = subprocess.Popen("docker build -t "+dockerBase+" .", shell=True)
    s.wait()
        
    # Leave all swarms before starting
    print("\n\n")
    init_cmd = docker + " swarm init"
    leave_cmd = docker + " swarm leave --force"
    for node in nodes: 
        print("Node", node[0], "leaving swarm...")
        sshAddr = node[2] + "@" + node[1]
        s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,leave_cmd])
        print("The commandline is {}".format(s1.args))
        s1.communicate()
    subprocess.run([leave_cmd], shell=True) #, check=True)
    subprocess.run([docker + " network rm " + clusterNet], shell=True) #, check=True)
    
    # Make nodes join swarm 
    srch = re.search('.*(docker swarm join .+)', subprocess.run(init_cmd, shell=True, capture_output=True, text=True).stdout) # create swarm and get command to join it
    if srch:
        join_cmd = srch.group(1)
        print("----join command: " + join_cmd)
        print("\n\n")
        for node in nodes:
            print("Node", node[0], "joining swarm...")
            sshAddr = node[2] + "@" + node[1]
            s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,join_cmd])
            print("the commandline is {}".format(s1.args))
            s1.communicate()
        net_cmd = docker + " network create --driver=overlay --attachable " + clusterNet
        subprocess.run([net_cmd], shell=True, check=True)
    else:
        print("----no join command")
        
    ## Start the containers (startRemoteContainers)
    for node in nodes: 
        
        # Stop and remove docker instance
        print("\n\n")
        instance = "server" + str(node[0])
        stop_cmd = docker + " stop " + instance
        rm_cmd   = docker + " rm " + instance
        sshAddr   = node[2] + "@" + node[1]
        s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,stop_cmd + "; " + rm_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
        
        # start the Docker instance
        #opt1 = "--expose=8760-9999"
        opt1 = "--expose="+str(8760+node[0])
        #opt1 = "-p "+str(8760+node[0])+":"+str(8760+node[0])
        opt2 = "--network=" + clusterNet
        opt3 = "--cap-add=NET_ADMIN"
        opt4 = "--name " + instance
        opts = " ".join([opt1, opt2, opt3, opt4])
        run_cmd = docker + " run -td " + opts + " " + dockerBase
        s2 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,run_cmd])
        print("the commandline is {}".format(s2.args))
        s2.communicate()
        
        # Extract the IP address of the container
        ip_cmd = docker + " inspect " + instance + " | jq '.[].NetworkSettings.Networks."+clusterNet+".IPAddress'"
        s5 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,ip_cmd])
        print("the commandline is {}".format(s5.args))
        s5.communicate()
        
        srch = re.search('\"(.+?)\"', subprocess.run(ip_cmd, shell=True, capture_output=True, text=True).stdout)
        if srch:
            out = srch.group(1)
            print("----container's address:" + out)
            ipsOfNodes[node[0]] = out
        else:
            print("----container's address: UNKNOWN")
        
        # set the network throughput and latency 
        DELAY_MS=10
        RATE_MBIT=1000
        BUF_PKTS=33
        BDP_BYTES=(DELAY_MS/1000.0)*(RATE_MBIT*1000000.0/8.0)
        BDP_PKTS=BDP_BYTES/1500
        LIMIT_PKTS=BDP_PKTS+BUF_PKTS
        # Local command: tc qdisc replace dev eth0 root netem delay ${DELAY_MS}ms rate ${RATE_MBIT}Mbit limit ${LIMIT_PKTS}
 
        print('Docker adding netem rule to node', node[0])
        tc_cmd = "tc qdisc add dev eth0 root netem delay " + str(DELAY_MS) + "ms rate "+str(RATE_MBIT)+"Mbit limit "+str(LIMIT_PKTS)
        lat_cmd = docker + " exec -t " + instance + " bash -c \"" + tc_cmd + "\""
        s4 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,lat_cmd])
        print("the commandline is {}".format(s4.args))
        s4.communicate()
        
    genLocalAddresses(numServers)
    
    ## run the experiment
    for node in nodes: 
        dockerI = "server" + str(node[0])
        sshAddr   = node[2] + "@" + node[1]
        
        srun = " ./server "+str(node[0])+" "+topofilename+" "+str(payloadSize)+" "+str(choice)+" "+str(numBcasts)+" "+str(sleepTime)+" "+str(minBidMeasure)+" "+str(maxBidMeasure)+" "+str(MBD_1)+" "+str(MBD_2)+" "+str(MBD_3)+" "+str(MBD_4)+" "+str(MBD_5)+" "+str(MBD_6)+" "+str(MBD_7)+" "+str(MBD_8)+" "+str(MBD_9)+" "+str(MBD_10)+" "+str(MBD_11)+" "+str(MBD_12)+" "+str(writingIntervals)
        
        run_cmd = docker + " exec -t " + dockerI + " bash -c \"" + srun + "\""
        s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,run_cmd])
        print("the commandline is {}".format(s1.args))
        
    time.sleep(experimentTime)
        
    ## stop and rm docker instances 
    
    for node in nodes:
        instance = "server" + str(node[0])
        stop_cmd = docker + " stop " + instance
        rm_cmd   = docker + " rm " + instance
        sshAddr   = node[2] + "@" + node[1]
        s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,stop_cmd + "; " + rm_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
# End of executeCluster
        
    # Cleanup: make nodes leave swarm
    print("\n\n")
    for node in nodes: 
        print("Node", node[0], "leaving swarm...")
        sshAddr = node[2] + "@" + node[1]
        s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,leave_cmd])
        print("The commandline is {}".format(s1.args))
        s1.communicate()
    subprocess.run([leave_cmd], shell=True) #, check=True)
    subprocess.run([docker + " network rm " + clusterNet], shell=True) #, check=True)
        
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
