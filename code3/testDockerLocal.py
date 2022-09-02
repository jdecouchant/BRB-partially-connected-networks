## Imports
import subprocess
import time
import sys
import getpass
import os
import argparse

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


numServers = 10
numFaulty = 1
connectivity = 3

payloadSize = 64 # in Bytes
numBcasts = 10# 00 # total num of broadcast per node
sleepTime = 5000000; # delay between 2 broadcasts in microsec
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 500; # higher bound broadcast id for stats
experimentTime = 60 # in seconds
writingIntervals = 1

resultsfilename = 'fig4.txt'

# 1 - Dolev, 2 - Bracha-Dolev, 3 - Opt Bracha-Dolev, 4 - Bracha-CPA, 5 - Opt Bracha-CPA
choice = 3

doPrintGraph = True

all_previous_to_true = True
all_to_true = True

MOD1 = False
MOD2 = False
MOD3 = False
MOD4 = False
MOD5 = False

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

if all_previous_to_true:
    MOD1 = True
    MOD2 = True
    MOD3 = True
    MOD4 = True
    MOD5 = True

if all_to_true:
    MBD_1 = True # Associate payload to ID to avoid resending + delete echo or ready sender with single hop
    MBD_2 = True # Single-hop Send msgs // PB with this option
    MBD_3 = True # Echo_Echo messages
    MBD_4 = True # Ready_Echo messages
    MBD_5 = True # Optimized Send messages 
    MBD_6 = True # Ignore Echos rcvd after corresponding Ready delivered 
    MBD_7 = True # Ignore all Echos rcvd after delivering content 
    MBD_8 = True # Do not send Echos to Ready neighbor
    MBD_9 = True # Do not send anything to neighbor that delivered 
    MBD_10 = True # Ignore msgs whose path is a superpath of known path 
    MBD_11 = True # Bracha overprovisioning - to improve in send_echo and send_ready 
    MBD_12 = True # Source sends its new Echo to only 2f+1 other processes

if connectivity * numServers % 2 == 1 or connectivity < 2*numFaulty+1:
    print('Incorrect connectivity')
    sys.exit()

# generate local addesses (for local experiments, use another function for distributed experiments)
def genLocalAddresses(numNodes):
    # we erase the content of the file
    open(addresses,'w').close()
    # write all addresses
    f = open(addresses,'a')
    for i in range(numNodes):
        host = "172.17.0.1"
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
        f.write('id:'+str(i)+' host:172.17.0.' + str(i+2) + ' port:'+str(8760+i)+'\n')
        nodes.append((i,host,username,pem,remotedir))
    f.close()

    print(">>>>>>>>>>>>>>>>>>> Generate graph")
    G = generateGraphs.generate_KRegularKConnectedRandomGraph(numServers,connectivity,numFaulty)

    if doPrintGraph:
        printGraph(G, topofilename, numServers, numFaulty)

    f = open(topofilename, 'r')
    t = f.readline().rstrip('\n').split(' ')
    numServers = int(t[0])
    numFaulty = int(t[1])
    f.close()

    #### build container:
##    s = subprocess.Popen("make clean; make", shell=True)
##    s.wait()

    s = subprocess.Popen("docker container stop `docker container ls -aq`", shell=True)
    s.wait()
    s = subprocess.Popen("docker container rm `docker container ls -aq`", shell=True)
    s.wait()

    s = subprocess.Popen("rm log/*", shell=True)
    s.wait()

    s = subprocess.Popen("docker build -t brachadolev .", shell=True)
    s.wait()
    
    # Create the required number of containers 
    for i in range(numServers):
##        cmd = """docker container run -tid --cap-add NET_ADMIN --network host brachadolev"""
        cmd='docker container run --name server'+str(i)+' -tid --cap-add NET_ADMIN '+ \
        '--ip 172.17.0.'+str(i+2)+' -p '+str(8760+i)+':'+str(8760+i)+' brachadolev'
        # the ip address is not changed...
        print(cmd)
        s = subprocess.Popen(cmd, shell=True)
        s.wait()

    containerIds = subprocess.check_output("docker container ls -aq", shell=True)
    containerIds = containerIds.decode("utf-8").split('\n')[:-1]
    print(containerIds)


    DELAY_MS=10
    RATE_MBIT=1000
    BUF_PKTS=33
    BDP_BYTES=(DELAY_MS/1000.0)*(RATE_MBIT*1000000.0/8.0)
    BDP_PKTS=BDP_BYTES/1500
    LIMIT_PKTS=BDP_PKTS+BUF_PKTS
 # tc qdisc replace dev eth0 root netem delay ${DELAY_MS}ms rate ${RATE_MBIT}Mbit limit ${LIMIT_PKTS}
 
    for i in range(numServers):
        print('Docker adding netem rule to node', i)
        cmd = "docker container exec "+containerIds[i]+" tc qdisc add dev eth0 root netem delay "+str(DELAY_MS)+"ms rate "+str(RATE_MBIT)+"Mbit limit "+str(LIMIT_PKTS)
##        cmd = "tc qdisc replace dev eth1 root netem delay "+str(DELAY_MS)+"ms rate "+str(RATE_MBIT)+"Mbit limit "+str(LIMIT_PKTS)
        s = subprocess.Popen(cmd, shell=True)
        s.wait()

    for i in range(numServers):
        print('Starting node', i)
        cmd = "docker container exec -t server"+str(i)+" ./server "
        cmd=cmd+str(i)+" "+topofilename+" "+str(payloadSize)+" "
        cmd=cmd+str(MOD1)+" "+str(MOD2)+" "+str(MOD3)+" "+str(MOD4)+" "+str(MOD5)+" "
        cmd=cmd+str(choice)+" "+str(numBcasts)+" "+str(sleepTime)+" "+str(minBidMeasure)+" "+str(maxBidMeasure)
        cmd=cmd+" "+str(MBD_1)+" "+str(MBD_2)+" "+str(MBD_3)+" "+str(MBD_4)+" "+str(MBD_5)+" "+str(MBD_6)+" "+str(MBD_7)+" "+str(MBD_8)+" "+str(MBD_9)+" "+str(MBD_10)+" "+str(MBD_11)+" "+str(MBD_12)
        cmd = cmd+" "+str(writingIntervals)
        s = subprocess.Popen(cmd, shell=True)

    time.sleep(experimentTime)

    for i in range(numServers):
        cmd = "docker container cp server"+str(i)+":/app/log/. log/" 
        s = subprocess.Popen(cmd, shell=True)
        s.wait()

##    subprocess.Popen("bash dockerImportResults.sh", shell=True)
##    s.wait()
    
    print('Stopping docker containers')
    s = subprocess.Popen("docker container stop `docker container ls -aq`", shell=True)
    s.wait()

    print('Removing docker containers')
    s = subprocess.Popen("docker container rm `docker container ls -aq`", shell=True)
    s.wait()

    #return

    #print(">>>>>>>>>>>>>>>>>>> Starting processes")
    #list_proc = []
    #for (i,host,username,pem,remotedir) in nodes:
        ## ./server nodeId topologyFile
        ##if i==0:
        ##s = Popen(["valgrind", "--tool=callgrind", "./server", str(i), topofilename, str(payloadSize), str(choice)])
        ##else:
        #if remote:
            #cmd    = "\"\"cd " + remotedir + " && ./server " + str(i) + " " + topofilename + " " + str(payloadSize) + " " + str(choice) + " " + str(numBcasts) + " " + str(sleepTime) + " " + str(minBidMeasure) + " " + str(maxBidMeasure) + " " + str(MBD_1) + " " + str(MBD_2) + " " + str(MBD_3) + " " + str(MBD_4) + " " + str(MBD_5) + " " + str(MBD_6) + " " + str(MBD_7) + " " + str(MBD_8) + " " + str(MBD_9) + " " + str(MBD_10) + " " + str(MBD_11) + " " + str(MBD_12) + " " + str(writingIntervals) + "\"\""
            ### TODO: we might want to copy the files there if they are not already there
            #sshAdr = username + "@" + host
            #if pem == "":
                #s = Popen(["ssh","-o",sshOpt1,"-ntt",sshAdr,cmd])
            #else:
                #s = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        #else:
            #print(str(i), topofilename, str(payloadSize), str(choice), str(numBcasts), str(sleepTime), str(minBidMeasure), str(maxBidMeasure), str(MBD_1), str(MBD_2), str(MBD_3), str(MBD_4), str(MBD_5), str(MBD_6), str(MBD_7), str(MBD_8), str(MBD_9), str(MBD_10), str(MBD_11), str(MBD_12), str(writingIntervals))
            #s = Popen(["./server", str(i), topofilename, str(payloadSize), str(choice), str(numBcasts), str(sleepTime), str(minBidMeasure), str(maxBidMeasure), str(MBD_1), str(MBD_2), str(MBD_3), str(MBD_4), str(MBD_5), str(MBD_6), str(MBD_7), str(MBD_8), str(MBD_9), str(MBD_10), str(MBD_11), str(MBD_12), str(writingIntervals)])
        #list_proc.append(s)

    #time.sleep(experimentTime) # sleeping time in seconds
    #print(">>>>>>>>>>>>>>>>>>> kill all processes")
    #for s in list_proc:
        #s.kill()
        
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

    print(">>>>>>>>>>>>>>>>>>> get number messages")
    [a,b,c,d] = getStats.getNumMsgs(numServers)
    print(a,b,c,d)

def stopAll():
    for (host,username,pem,remotedir) in hosts:
        if remote:
            cmd    = "\"\"killall server\"\""
            sshAdr = username + "@" + host
            if pem == "":
                s = Popen(["ssh","-o",sshOpt1,"-ntt",sshAdr,cmd])
            else:
                s = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        else:
            s = Popen(["killall", "server"])




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
