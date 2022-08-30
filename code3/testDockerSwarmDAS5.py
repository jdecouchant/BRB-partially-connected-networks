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
    
    os.system("preserve -long-list > preserveLongList.txt")
    # Get the names of the machines that have been reserved
    hostnames = []
    f = open('preserveLongList.txt', 'r')
    for l in f:
        l = l.split('\t')
        if len(l) > 2:
            l[1] = l[1].rstrip(' ')
            if l[1] == 'jdecouch':
                hostnames = l[8].rstrip('\n').split(' ')

                nhostnames = []
                for hn in hostnames:
                    if hn != 'node308' and hn != 'node301' and hn != 'node305':
                        nhostnames.append(hn)
                hostnames = nhostnames
                print('Found hosts = ', hostnames)
                break
    f.close()
    
    username  = getpass.getuser()
    pem       = "bcast.pem"
    hosts = []
    for hn in hostnames:
        hosts.append((hn, username, pem, curdir))
    print('hosts =', hosts)

    for i in range(numServers):
        remotedir = curdir
        # we cycle through the hosts
        (host,username,pem,remotedir) = hosts[i % len(hostnames)]
        hosts.append((host,username,pem,remotedir))
        nodes.append((i,host,username,pem,remotedir))
    
    print(nodes)

    print(">>>>>>>>>>>>>>>>>>> Generate network topology")
    G = generateGraphs.generate_KRegularKConnectedRandomGraph(numServers,connectivity,numFaulty)
    printGraph(G, topofilename, numServers, numFaulty)

    sshOpt1  = "StrictHostKeyChecking=no"
    sshOpt2  = "ConnectTimeout=10"
    
    #s = subprocess.Popen("docker build -t "+dockerBase+" .", shell=True)
    #s.wait()
    
    list_p = []
    for hn in hostnames: 
        print('Load docker image on ', hn)
        load_cmd = "sudo "+docker+" load < brachadolev.tar.gz"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",hn,load_cmd])
        list_p.append(s1)
        #s1.communicate()
    for p in list_p:
        p.wait()
    
    # Have replicas leave all swarms 
    print("\n\n")
    leave_cmd = "sudo " + docker + " swarm leave --force"
    for hn in hostnames: 
        print("Connecting to", hn, "to leave swarm...")
        #sshAddr = node[2] + "@" + node[1]
        #sshAddr = node[1]
        #s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,leave_cmd])
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",hn,leave_cmd])
        print("The commandline is {}".format(s1.args))
        s1.communicate()
    
    # Stop and remove docker instances
    print("\n\n")
    #list_p = []
    for node in nodes: 
        instance = "server" + str(node[0])
        stop_cmd = "sudo " + docker + " stop " + instance
        rm_cmd   = "sudo " + docker + " rm -f " + instance
        sshAddr   = node[1]
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,stop_cmd + "; " + rm_cmd])
        #list_p.append(s1)
        print("commandline: {}".format(s1.args))
        s1.communicate()
    #for p in list_p:
        #p.wait()
        
    # Disconnect endpoints 
    print("\n\n")
    list_p = []
    for node in nodes: 
        instance = "server" + str(node[0])
        stop_cmd = "sudo " + docker + " network disconnect --force " + clusterNet + " " + instance
        sshAddr   = node[1]
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,stop_cmd])
        print("commandline: {}".format(s1.args))
        #s1.communicate()
        list_p.append(s1)
    for p in list_p:
        p.wait()
        
    #sys.exit()
    
    # Create swarm 
    s = Popen(["ssh","-o",sshOpt1,"-ntt","localhost", "sudo " + docker + " swarm init --advertise-addr eth0"], stdout=subprocess.PIPE, stderr=subprocess.PIPE) # for node301
    out, err = s.communicate()
    out = out.decode("utf-8").split('\n')
    print('out', out)
    
    # Make nodes join swarm 
    join_cmd = out[4].rstrip("\\") + out[5].rstrip("\\") + out[6].rstrip("\\")
    join_cmd = join_cmd.lstrip(' ')
    join_cmd = join_cmd.replace("\\\r", "")
    join_cmd = join_cmd.replace('\r', '')
    join_cmd = "sudo " + join_cmd
    
    print("\n\n")
    print("----join command: " + join_cmd)
    for hn in hostnames[1:]: 
        print("Node", hn, "joining swarm...")
        #sshAddr = node[2] + "@" + node[1]
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",hn,join_cmd])
        print("commandline: {}".format(s1.args))
        s1.communicate()
    
    print("\n\n")
    print('Create network overlay')
    s = Popen(["ssh","-o",sshOpt1,"-ntt","localhost", "sudo " + docker + " network create --driver=overlay --attachable " + clusterNet]) 
    out, err = s.communicate()
    
    ### Start the containers (startRemoteContainers)
    print("\n\n")
    for node in nodes: 
        instance = "server" + str(node[0])
        sshAddr = node[1]
        # start the Docker instance
        #opt1 = "--expose=8760-9999"
        opt1 = "--expose="+str(firstPort)+"-"+str(firstPort+200)
        #opt1 = "--expose="+str(8760+node[0])
        #opt1 = "-p "+str(8760+node[0])+":"+str(8760+node[0])
        opt2 = "--network=" + clusterNet
        opt3 = "--cap-add=NET_ADMIN"
        opt4 = "--name " + instance
        opts = " ".join([opt1, opt2, opt3, opt4])
        run_cmd = "sudo " + docker + " run -td " + opts + " " + dockerBase
        s2 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,run_cmd])
        print("commandline: {}".format(s2.args))
        s2.communicate()
        
    print("\n\n")
    for node in nodes: 
        instance = "server" + str(node[0])
        sshAddr   = node[1]
        #ins_cmd = "sudo " + docker + " inspect --format='{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' " + instance
        ins_cmd = "sudo "+docker+" inspect --format '{{ .NetworkSettings.Networks.brachaDolev.IPAMConfig.IPv4Address }}' "+instance
        s = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr, ins_cmd ], stdout=subprocess.PIPE, stderr=subprocess.PIPE) 
        print("commandline: {}".format(s.args))
        out, err = s.communicate()
        out = out.decode("utf-8").rstrip('\r\n')
        print('Found IP address =', out)
        ipsOfNodes[node[0]] = out
        
    #sys.exit()
        
        #srch = re.search('\"(.+?)\"', subprocess.run(ip_cmd, shell=True, capture_output=True, text=True).stdout)
        #if srch:
            #out = srch.group(1)
            #print("----container's address:" + out)
            #ipsOfNodes[node[0]] = out
        #else:
            #print("----container's address: UNKNOWN")
            
    #list_p = []
    #for node in nodes: 
        #sshAddr   = node[2] + "@" + node[1]
        #instance = "server" + str(node[0])
        ## set the network throughput and latency 
        #DELAY_MS=1
        #RATE_MBIT=1000
        #BUF_PKTS=33
        #BDP_BYTES=(DELAY_MS/1000.0)*(RATE_MBIT*1000000.0/8.0)
        #BDP_PKTS=BDP_BYTES/1500
        #LIMIT_PKTS=math.floor(BDP_PKTS+BUF_PKTS)
        ## Local command: tc qdisc replace dev eth0 root netem delay ${DELAY_MS}ms rate ${RATE_MBIT}Mbit limit ${LIMIT_PKTS}
 
        #print('Docker adding netem rule to node', node[0])
        #tc_cmd = "tc qdisc add dev eth0 root netem delay " + str(DELAY_MS) + "ms rate "+str(RATE_MBIT)+"Mbit limit "+str(LIMIT_PKTS)
        #lat_cmd = "sudo " + docker + " exec -t " + instance + " bash -c \"" + tc_cmd + "\""
        #s4 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,lat_cmd])
        #print("commandline: {}".format(s4.args))
        #list_p.append(s4)
        ##s4.communicate()
    #for p in list_p:
        #p.wait()
        
    print(">>>>>>>>>>>>>>>>>>> Generate addresses")
    genLocalAddresses(numServers)
    
    print("\n\n")
    list_p = []
    for node in nodes:
        dockerI = "server" + str(node[0])
        sshAddr   = node[1]
        cp_cmd = "sudo "+docker+" cp ~/addresses "+dockerI+":/app/"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,cp_cmd])
        print("commandline: {}".format(s1.args))
        list_p.append(s1)
        
        cp_cmd = "sudo "+docker+" cp ~/topology.txt "+dockerI+":/app/"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,cp_cmd])
        print("commandline: {}".format(s1.args))
        list_p.append(s1)
    for p in list_p:
        p.wait()
        
    time.sleep(10)
    #sys.exit()

    ## run the experiment
    for node in nodes: 
        dockerI = "server" + str(node[0])
        sshAddr   = node[1]
        
        srun = " ./server "+str(node[0])+" "+topofilename+" "+str(payloadSize)+" "+str(choice)+" "+str(numBcasts)+" "+str(sleepTime)+" "+str(minBidMeasure)+" "+str(maxBidMeasure)+" "+str(MBD_1)+" "+str(MBD_2)+" "+str(MBD_3)+" "+str(MBD_4)+" "+str(MBD_5)+" "+str(MBD_6)+" "+str(MBD_7)+" "+str(MBD_8)+" "+str(MBD_9)+" "+str(MBD_10)+" "+str(MBD_11)+" "+str(MBD_12)+" "+str(writingIntervals)
        
        run_cmd = "sudo " + docker + " exec -t " + dockerI + " bash -c \"" + srun + "\""
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,run_cmd])
        print("commandline: {}".format(s1.args))
    
    time.sleep(experimentTime)
    
    print("\n\n")
    for node in nodes:
        dockerI = "server" + str(node[0])
        sshAddr   = node[1]
        cp_cmd = "sudo "+docker+" cp "+dockerI+":/app/log/broadcastTimes_"+str(node[0])+" ~/log/"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,cp_cmd])
        print("commandline: {}".format(s1.args))
        
        cp_cmd = "sudo "+docker+" cp "+dockerI+":/app/log/bytesCounts_"+str(node[0])+" ~/log/"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,cp_cmd])
        print("commandline: {}".format(s1.args))
        
        cp_cmd = "sudo "+docker+" cp "+dockerI+":/app/log/deliver_"+str(node[0])+" ~/log/"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,cp_cmd])
        print("commandline: {}".format(s1.args))
        
        cp_cmd = "sudo "+docker+" cp "+dockerI+":/app/log/msgCounts_"+str(node[0])+" ~/log/"
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,cp_cmd])
        print("commandline: {}".format(s1.args))
    
    # Have replicas leave all swarms 
    print("\n\n")
    leave_cmd = "sudo " + docker + " swarm leave --force"
    for node in nodes: 
        print("Connecting to", node[0], "to leave swarm...")
        #sshAddr = node[2] + "@" + node[1]
        sshAddr = node[1]
        #s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,leave_cmd])
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,leave_cmd])
        print("The commandline is {}".format(s1.args))
        s1.communicate()
    
    # Stop and remove docker instances
    print("\n\n")
    list_p = []
    for node in nodes: 
        instance = "server" + str(node[0])
        stop_cmd = "sudo " + docker + " stop " + instance
        rm_cmd   = "sudo " + docker + " rm -f " + instance
        sshAddr   = node[1]
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,stop_cmd + "; " + rm_cmd])
        print("commandline: {}".format(s1.args))
        list_p.append(s1)
        #s1.communicate()
    for p in list_p:
        p.wait()
        
    # Disconnect endpoints 
    print("\n\n")
    for node in nodes: 
        instance = "server" + str(node[0])
        stop_cmd = "sudo " + docker + " network disconnect --force " + clusterNet + " " + instance
        sshAddr   = node[1]
        s1 = Popen(["ssh","-o",sshOpt1,"-ntt",sshAddr,stop_cmd])
        print("commandline: {}".format(s1.args))
        s1.communicate()
        
    #sys.exit()
        
    # Cleanup: make nodes leave swarm
    #print("\n\n")
    #leave_cmd = "sudo " + docker + " swarm leave --force"
    #for node in nodes: 
        #print("Node", node[0], "leaving swarm...")
        #sshAddr = node[2] + "@" + node[1]
        #s1 = Popen(["ssh","-i",node[3],"-o",sshOpt1,"-ntt",sshAddr,leave_cmd])
        #print("The commandline is {}".format(s1.args))
        #s1.communicate()
    #subprocess.run([leave_cmd], shell=True) #, check=True)
    #subprocess.run([docker + " network rm " + clusterNet], shell=True) #, check=True)
        
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
