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

if len(sys.argv) < 27: 
    print('Not enough args')
    
print(str(sys.argv))

numServers = int(sys.argv[1]) #10
print('Num servers = ', numServers)

numFaulty = int(sys.argv[2]) #1

connectivity = int(sys.argv[3]) #3

payloadSize = int(sys.argv[4]) #16 # in Bytes

numBcasts = int(sys.argv[5]) # 10 # total num of broadcast per node
sleepTime = int(sys.argv[6]) #5000000; # delay between 2 broadcasts in microsec
minBidMeasure = 0; # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 500; # higher bound broadcast id for stats
experimentTime = int(sys.argv[7]) #60 # in seconds
writingIntervals = int(sys.argv[8]) #1

# 1 - Dolev, 2 - Bracha-Dolev, 3 - Opt Bracha-Dolev, 4 - Bracha-CPA, 5 - Opt Bracha-CPA
choice = 3

MOD1 = bool(int(sys.argv[9])) #False
MOD2 = bool(int(sys.argv[10])) #False
MOD3 = bool(int(sys.argv[11])) #False
MOD4 = bool(int(sys.argv[12])) #False
MOD5 = bool(int(sys.argv[13])) #False

MBD_1 = bool(int(sys.argv[14])) #False # Associate payload to ID to avoid resending + delete echo or ready sender with single hop
MBD_2 = bool(int(sys.argv[15])) #False # Single-hop Send msgs // PB with this option
MBD_3 = bool(int(sys.argv[16])) #False # Echo_Echo messages
MBD_4 = bool(int(sys.argv[17])) #False # Ready_Echo messages
MBD_5 = bool(int(sys.argv[18])) #False # Optimized Send messages 
MBD_6 = bool(int(sys.argv[19])) #False # Ignore Echos rcvd after corresponding Ready delivered 
MBD_7 = bool(int(sys.argv[20])) #False # Ignore all Echos rcvd after delivering content 
MBD_8 = bool(int(sys.argv[21])) #False # Do not send Echos to Ready neighbor
MBD_9 = bool(int(sys.argv[22])) #False # Do not send anything to neighbor that delivered 
MBD_10 = bool(int(sys.argv[23])) #False # Ignore msgs whose path is a superpath of known path 
MBD_11 = bool(int(sys.argv[24])) #False # Bracha overprovisioning - to improve in send_echo and send_ready 
MBD_12 = bool(int(sys.argv[25])) #False # Source sends its new Echo to only 2f+1 other processes

resultsfilename = sys.argv[26] #'results.txt'
doPrintGraph = bool(int(sys.argv[27]))

asyncNet = False
if len(sys.argv) > 28:
    asyncNet = bool(int(sys.argv[28]))

def parametersToStr(numServers, numFaulty, connectivity, payloadSize):
    return str(numServers)+'\t'+str(numFaulty)+'\t'+str(connectivity)+'\t'+str(payloadSize)
    
def modBonomiToStr(MOD1, MOD2, MOD3, MOD4, MOD5):
    return str(MOD1)+'\t'+str(MOD2)+'\t'+str(MOD3)+'\t'+str(MOD4)+'\t'+str(MOD5)

def modToStr(MBD_1, MBD_2, MBD_3, MBD_4, MBD_5, MBD_6, MBD_7, MBD_8, MBD_9, MBD_10, MBD_11, MBD_12):
    return str(MBD_1)+'\t'+str(MBD_2)+'\t'+str(MBD_3)+'\t'+str(MBD_4)+'\t'+str(MBD_5)+'\t'+str(MBD_6)+'\t'+str(MBD_7)+'\t'+str(MBD_8)+'\t'+str(MBD_9)+'\t'+str(MBD_10)+'\t'+str(MBD_11)+'\t'+str(MBD_12)+'\t'

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


    DELAY_MS=50
    DELAY_2_MS=50 
    RATE_MBIT=10 # Was 1000
    BUF_PKTS=33
    BDP_BYTES=(DELAY_MS/1000.0)*(RATE_MBIT*1000000.0/8.0)
    BDP_PKTS=BDP_BYTES/1500
    LIMIT_PKTS=BDP_PKTS+BUF_PKTS
 # tc qdisc replace dev eth0 root netem delay ${DELAY_MS}ms rate ${RATE_MBIT}Mbit limit ${LIMIT_PKTS}
 
    for i in range(numServers):
        print('Docker adding netem rule to node', i)
        if asyncNet:
            cmd = "docker container exec "+containerIds[i]+" tc qdisc add dev eth0 root netem delay "+str(DELAY_MS)+"ms "+str(DELAY_2_MS)+"ms distribution normal rate "+str(RATE_MBIT)+"Mbit limit "+str(LIMIT_PKTS)
        else:
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
        
    resFile = open(resultsfilename, 'a')
    
    resFile.write(parametersToStr(numServers, numFaulty, connectivity, payloadSize)+'\t')
    resFile.write(modBonomiToStr(MOD1, MOD2, MOD3, MOD4, MOD5)+'\t')
    resFile.write(modToStr(MBD_1, MBD_2, MBD_3, MBD_4, MBD_5, MBD_6, MBD_7, MBD_8, MBD_9, MBD_10, MBD_11, MBD_12)+'\n')
        
    numEffectiveBcasts = getStats.getNumEffectiveBroadcasts(numServers)
    resFile.write('Num effective bcasts: '+str(numEffectiveBcasts)+'\n')
    
    print(">>>>>>>>>>>>>>>>>>> get num delivers")
    numDelivers = getStats.getNumDelivers(numServers)
    resFile.write('Num delivers: ')
    for x in numDelivers: 
        print(str(x/numEffectiveBcasts*100)+'%', end=' ')
        resFile.write(str(x/numEffectiveBcasts*100)+'%'+'\t')
    print('')
    resFile.write('\n')

    print(">>>>>>>>>>>>>>>>>>> get latency")
    [a,b,c,d,e] = getStats.getLatencies(numServers)
    print(a,b,c,d,e)
    resFile.write('Latency: '+str(a)+' '+str(b)+' '+str(c)+' '+str(d)+' '+str(e)+'\n')
    
    print(">>>>>>>>>>>>>>>>>>> get throughput")
    [a,b,c] = getStats.getThroughput(numServers)
    print(a,b,c)
    #resFile.write('Throughput: '+str(a)+' '+str(b)+' '+str(c)+'\n')

    print(">>>>>>>>>>>>>>>>>>> get network consumption")
    [a,b,c,d] = getStats.getNetworkConsumption(numServers)
    print(a,b,c,d)
    resFile.write('Net consumption: '+str(a)+' '+str(b)+' '+str(c)+' '+str(d)+'\n')

    print(">>>>>>>>>>>>>>>>>>> get number messages")
    [a,b,c,d] = getStats.getNumMsgs(numServers)
    print(a,b,c,d)
    resFile.write('Num messages: '+str(a)+' '+str(b)+' '+str(c)+' '+str(d)+'\n\n')
    
    resFile.close()

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


runExperiment()

#parser = argparse.ArgumentParser(description='Bracha-Dolev')
#parser.add_argument("--start", action="store_true")
#parser.add_argument("--stop",  action="store_true")
#args = parser.parse_args()


#if args.start:
    #print("lauching experiment")
    #runExperiment()
#elif args.stop:
    #print("stoping experiment")
    #stopAll()
#else:
    #print("lauching experiment")
    #runExperiment()
