import sys
import random
import os
import subprocess
import pickle
import threading
import time
from matplotlib import pyplot as plt
import numpy as np
from ctypes import c_double
import os.path
import math
import sys
import queue


import networkx as nx

import kdiamond
import kpasted
import generateGraphs

from statistics import mean


def printOmnetLinks(G, N):
    s = ''
    for i in range(N):
        for j in range(i, N):
            if G.has_edge(i, j):
                s += 'cli['+str(i)+'].pppg++ <--> ethernetline <--> cli['+str(j)+'].pppg++;\n'
    return s

def nedname(N, K, F):
    return 'random_'+str(N)+'_'+str(K)+'_'+str(F)+'.ned'

def ininame(N, K, F):
    return 'random_'+str(N)+'_'+str(K)+'_'+str(F)+'.ini'

def prefix(N, K, F):
    return str(N)+'_'+str(K)+'_'+str(F)

def writeNedFile(G, N, K, F):

    # Generate the random.ned file
    fo = open(nedname(N,K,F), 'w')
    fo.write('\
    package broadcastsign.simulations;\n\
    \n\
    import inet.common.misc.ThruputMeteringChannel;\n\
    import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;\n\
    import inet.node.inet.Router;\n\
    import inet.node.inet.StandardHost;\n\n')
    fo.write('network random_'+prefix(N,K,F)+'\n\
    {\n\
        parameters:\n\
            int n;\n\
        types:\n\
            channel fiberline extends ThruputMeteringChannel\n\
            {\n\
                delay = 1us;\n\
                datarate = 512Mbps;\n\
                thruputDisplayFormat = "u";\n\
            }\n\
            channel ethernetline extends ThruputMeteringChannel\n\
            {\n\
                delay = 0.5ms;\n\
                datarate = 1Mbps;\n\
                thruputDisplayFormat = "u";\n\
            }\n\
        submodules:\n\
            configurator: Ipv4NetworkConfigurator {\n\
                parameters:\n\
                    @display("p=100,100;is=s");\n\
            }\n\
            cli[n]: StandardHost {\n\
                parameters:\n\
                    @display("i=device/laptop"); //;p=250,100,col,100");\n\
            }\n\
        connections:\n\
    ')
    fo.write(printOmnetLinks(G,N))
    fo.write('}')
    fo.close()

def writeIniFile(N, K, F):
    
    fo = open(ininame(N,K,F), 'w')
    fo.write('[General]\n')

    fo.write('cmdenv-event-banners = false'+'\n')
    fo.write('**.cmdenv-log-level = OFF'+'\n')
    fo.write('cmdenv-performance-display = false'+'\n')
    fo.write('cmdenv-status-frequency = 1s'+'\n')
    fo.write('cmdenv-express-mode = true'+'\n')
    fo.write('fname-append-host = false'+'\n')
    fo.write('parallel-simulation = false'+'\n')
    fo.write('print-undisposed = false'+'\n')
    fo.write('record-eventlog = false'+'\n')
    fo.write('warnings = false'+'\n')
    fo.write('**.module-eventlog-recording = false'+'\n')
    fo.write('*.result-recording-modes = -'+'\n')
    fo.write('**.scalar-recording = false'+'\n')
    fo.write('**.vector-record-eventnumbers = false'+'\n')
    fo.write('**.vector-recording = false'+'\n')
    fo.write('**.statistic-recording = false'+'\n')

    fo.write('**.ppp[*].queue.frameCapacity = 0'+'\n')
    fo.write('**.ppp[*].ppp.queue.typename = "DropTailQueue"'+'\n\n')
    
    fo.write('**.cli[*].app[*].startTimePar = 0.03s'+'\n\n')
                
    cname = prefix(N,K,F)+'-allOptim'
    fo.write('[Config c'+prefix(N,K,F)+'-allOptim]\n')
    fo.write('**.OPTIM_SENDTOECHO = true'+'\n');
    fo.write('**.OPTIM_ECHOTOREADY = true'+'\n');
    fo.write('**.OPTIM_CUTECHOREADY_AFTER_DELIVER = true'+'\n');
    fo.write('**.OPTIM_CUT_ECHO_AFTER_READY_RECEIVED = true'+'\n');
    fo.write('**.OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED = true'+'\n');
    fo.write('**.OPTIM_CUT_ECHO_TO_READY_NEIGHBORS = true'+'\n');
    fo.write('**.OPTIM_BRACHA_OVERPROVISIONING = true'+'\n');
    fo.write('**.OPTIM_BITSET = true'+'\n');
    fo.write('network = random_'+prefix(N,K,F)+'\n')
    fo.write('*.n = '+str(N)+'\n')
    fo.write('**.cli[*].app[*].nPar = '+str(N)+'\n')
    fo.write('**.cli[*].app[*].fPar = '+str(F)+'\n')
    fo.write('**.cli[*].app[*].kPar = '+str(K)+'\n')
    fo.write('**.cli[*].app[*].byzStringPar = ""'+'\n')
    fo.write('**.cli[*].numApps = 1'+'\n')
    fo.write('**.cli[*].app[*].typename = "PeerHost"'+'\n')
    fo.write('**.cli[*].app[*].localPort = 9999'+'\n')
    fo.write('**.cli[*].app[*].startTimePar = 0.03s'+'\n')
    fo.write('**.cli[*].app[*].msgSizePar = 128'+'\n')
    fo.write('**.cli[*].app[*].outputPrefixPar = "stats/'+prefix(N,K,F)+'"'+'\n')
    fo.write('\n')

    cname = prefix(N,K,F)+'-noOptim'
    fo.write('[Config c'+prefix(N,K,F)+'-noOptim]\n')
    fo.write('network = random_'+prefix(N,K,F)+'\n')
    fo.write('*.n = '+str(N)+'\n')
    fo.write('**.cli[*].app[*].nPar = '+str(N)+'\n')
    fo.write('**.cli[*].app[*].fPar = '+str(F)+'\n')
    fo.write('**.cli[*].app[*].kPar = '+str(K)+'\n')
    fo.write('**.cli[*].app[*].byzStringPar = ""'+'\n')
    fo.write('**.cli[*].numApps = 1'+'\n')
    fo.write('**.cli[*].app[*].typename = "PeerHost"'+'\n')
    fo.write('**.cli[*].app[*].localPort = 9999'+'\n')
    fo.write('**.cli[*].app[*].msgSizePar = 128'+'\n')
    fo.write('**.cli[*].app[*].outputPrefixPar = "stats/'+prefix(N,K,F)+'"'+'\n')
    fo.write('\n')

    fo.close()

subprocess.run("rm dic.txt msgsImprovement.txt bitsImprovement.txt latsImprovement.txt", shell=True)

resd = {}
msgsImprovement = {}

bitsImprovement = {}

latsImprovement = {}

#if os.path.isfile('dic.txt'):
#    inf = open("dic.txt", "rb")
#    resd = pickle.load(inf)
#    inf.close()

repeats = 10
nthreads = 4
delete_ini_ned_file = True
    
lock = threading.Lock()

def runParams(nthreads):
    
    q = queue.Queue()
    for n in [22]: #range(10, 100, 6): # number of nodes
        resd[n] = {}
        msgsImprovement[n] = {}
        bitsImprovement[n] = {}
        latsImprovement[n] = {}

        for f in [8]: #range(1, int((n-1)/3)+1, 2): # number of byzantine nodes
            resd[n][f] = {}
            msgsImprovement[n][f] = {}
            bitsImprovement[n][f] = {}
            latsImprovement[n][f] = {}
            start = 2*f+1
            if (n * start % 2 == 1):
                start += 1
            for k in [start]: #range(start, n, 2): #range(2*f+1, n-1): # graph connectivity

                if k*n % 2 == 1:
                    continue
                resd[n][f][k] = {}
                resd[n][f][k][0] = []
                resd[n][f][k][1] = []
                msgsImprovement[n][f][k] = []
                bitsImprovement[n][f][k] = []
                latsImprovement[n][f][k] = []

                q.put([n, f, k])


    def worker():
        while True:
            lock.acquire()
            config = q.get()
            lock.release()
            if config is None:
                return

            n = config[0]
            f = config[1]
            k = config[2]

            subprocess.run('cp disjointPaths.py ./stats/'+prefix(n,k,f)+'.py', shell=True)
            
            l0_long = []
            l1_long = []

            b0_long = []
            b1_long = []
            
            success = 0

            while success < repeats:
                
                try:
                    # N*K must be even
                    G = generateGraphs.generate_KRegularKConnectedRandomGraph(n,k,f)

                    cname='c'+prefix(n,k,f)+'-noOptim'
                    subprocess.run('rm stats/'+prefix(n,k,f)+'*', shell=True)
                
                    subprocess.run('cp disjointPaths.py ./stats/'+prefix(n,k,f)+'.py', shell=True)
                
                    writeNedFile(G, n, k, f)
                    writeIniFile(n, k, f)
               
                    #cmd='../BroadcastSign -m -u Cmdenv -c '+cname+\
                    # ' -n ../src:.:../../inet4/src:../../inet4/examples:../../inet4/tutorials:../../inet4/showcases --image-path=../../inet4/images -l ../../inet4/src/INET '+\
                    # ininame(n,k,f) + ' > /dev/null'

                    cmd='../BRB -r 0 -m -u Cmdenv -c '+cname+' -n ../src:.:../../../../../sim/inet4/src:../../../../../sim/inet4/examples:../../../../../sim/inet4/tutorials:../../../../../sim/inet4/showcases --image-path=../../../../../sim/inet4/images -l ../../../../../sim/inet4/src/INET '\
                     +ininame(n,k,f)+' > /dev/null'
                    #print(cmd)
                    #sys.exit()
                    subprocess.call(cmd, shell=True)

                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.delivers', 'r')
                    ndelivers = 0
                    for line in inf:
                        ndelivers = int(line.rstrip('\n'))
                    if ndelivers != n:
                        print('Not all nodes delivered (no optim)! aborting')
                        print("n =",n,", f =",f,", k =",k,'no optim')
                        print(ndelivers, "out of", n) 
                        sys.exit()
                    inf.close()

                    cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-noOptim'
                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.msgs', 'r')
                    l0 = []
                    for line in inf:
                        l0.append(float(line.rstrip('\n')))
                    inf.close()
                    l0_long.extend(l0)

                    cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-noOptim'
                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.bits', 'r')
                    b0 = []
                    for line in inf:
                        b0.append(float(line.rstrip('\n')))
                    inf.close()
                    b0_long.extend(b0)

                    cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-noOptim'
                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.lat', 'r')
                    lat0 = []
                    for line in inf:
                        lat0.append(float(line.rstrip('\n')))
                    inf.close()

                    cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-allOptim'
                    subprocess.run('rm stats/'+str(n)+'_'+str(k)+'_'+str(f)+'*', shell=True)
                
                    subprocess.run('cp disjointPaths.py ./stats/'+prefix(n,k,f)+'.py', shell=True)

                    writeNedFile(G, n, k, f)
                    writeIniFile(n, k, f)
                    cmd='../BRB -r 0 -m -u Cmdenv -c '+cname+' -n ../src:.:../../../../../sim/inet4/src:../../../../../sim/inet4/examples:../../../../../sim/inet4/tutorials:../../../../../sim/inet4/showcases --image-path=../../../../../sim/inet4/images -l ../../../../../sim/inet4/src/INET '\
                      +ininame(n,k,f)+' > /dev/null'
                    #cmd='../BroadcastSign -m -u Cmdenv -c '+cname+\
                    # ' -n ../src:.:../../inet4/src:../../inet4/examples:../../inet4/tutorials:../../inet4/showcases --image-path=../../inet4/images -l ../../inet4/src/INET '\
                    # + ininame(n,k,f) + ' > /dev/null'
                    subprocess.call(cmd, shell=True)

                    ndelivers = 0
                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.delivers', 'r')
                    for line in inf:
                        ndelivers = int(line.rstrip('\n'))
                    if ndelivers != n:
                        print('Not all nodes delivered (no optims)! aborting')
                        print("n =",n,", f =",f,", k =",k,'all optims')
                        print(ndelivers, "out of", n)
                        sys.exit()
                    else:
                        if delete_ini_ned_file:
                            subprocess.run('rm '+ininame(n,k,f)+' '+nedname(n,k,f), shell=True)
                    inf.close()

                    cname='c'+prefix(n,k,f)+'-allOptim'
                    inf = open('stats/'+prefix(n,k,f)+'.msgs', 'r')
                    l1 = []
                    for line in inf:
                        l1.append(float(line.rstrip('\n')))
                    inf.close()
                    l1_long.extend(l1)

                    cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-allOptim'
                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.bits', 'r')
                    b1 = []
                    for line in inf:
                        b1.append(float(line.rstrip('\n')))
                    inf.close()
                    b1_long.extend(b1)

                    cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-allOptim'
                    inf = open('stats/'+str(n)+'_'+str(k)+'_'+str(f)+'.lat', 'r')
                    lat1 = []
                    for line in inf:
                        lat1.append(float(line.rstrip('\n')))
                    inf.close()

                    lock.acquire()
                    msgsImprovement[n][f][k].append( (mean(l1)-mean(l0)) / mean(l0))
                    bitsImprovement[n][f][k].append( (mean(b1)-mean(b0)) / mean(b0))
                    latsImprovement[n][f][k].append( (mean(lat1)-mean(lat0)) / mean(lat0))
                
                    print("n ="+str(n)+", f="+str(f)+", k="+str(k)+', r='+str(success),end='')
                    print(' - msgs :'+str(int(100*(mean(l1)-mean(l0)) / mean(l0)))+'% ',end='')
                    print('('+str(mean(l1))+', '+str(mean(l0))+')')
                    print(' - bits :'+str(int(100*(mean(b1)-mean(b0)) / mean(b0)))+'% ',end='')
                    print('('+str(mean(b1))+', '+str(mean(b0))+')')
                    print(' - latency :'+str(int(100*(mean(lat1)-mean(lat0)) / mean(lat0)))+'% ',end='')
                    print('('+str(mean(lat1))+', '+str(mean(lat0))+')')
                    lock.release()

                    subprocess.run('rm stats/'+prefix(n,k,f)+'*.graph', shell=True)
                except FileNotFoundError:
                    print('Caught error: continue')
                    continue
                success+=1

            lock.acquire()
            resd[n][f][k][1] = l1_long
            resd[n][f][k][0] = l0_long
            outf = open("dic.txt", "wb")
            pickle.dump(resd, outf)
            outf.close()
        
            outf = open("msgsImprovement.txt", "wb")
            pickle.dump(msgsImprovement, outf)
            outf.close()

            outf = open("bitsImprovement.txt", "wb")
            pickle.dump(bitsImprovement, outf)
            outf.close()

            outf = open("latsImprovement.txt", "wb")
            pickle.dump(latsImprovement, outf)
            outf.close()
            lock.release()

    threads = [ threading.Thread(target=worker) for _i in range(nthreads) ]
    for thread in threads:
        q.put(None)  # one EOF marker for each thread
        thread.start()

    for thread in threads:
        thread.join()



runParams(nthreads)

##
##            lock.acquire()
##            resd[n][f][k][0] = l0_long
##            resd[n][f][k][1] = l1_long
##            lock.release()
##
##            l0_long = []
##            l1_long = []
##            for r in range(repeats):
##                print('\t', r, ':', end='')
##
##                # N*K must be even
##                G = generateGraphs.generate_KRegularKConnectedRandomGraph(n,k)
##
##                cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-noOptim'
##                cmd = 'rm stats/output_'+cname[1:]+'*'                
##                out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
##                writeNedFile(G, n, k)
##                writeIniFile(n, k)
##                ininame = 'random_'+str(n)+'_'+str(k)+'.ini'
##                cmd='../BRB -r 0 -m -u Cmdenv -c '+cname+' -n ../src:.:../../../../../sim/inet4/src:../../../../../sim/inet4/examples:../../../../../sim/inet4/tutorials:../../../../../sim/inet4/showcases --image-path=../../../../../sim/inet4/images -l ../../../../../sim/inet4/src/INET '+ininame+' > /dev/null'
##                os.system(cmd)
##
##                ndelivers = 0
##                inf = open('stats/output_'+cname[1:]+'.delivers', 'r')
##                for line in inf:
##                    ndelivers = int(line.rstrip('\n'))
##                if ndelivers != n:
##                    print('Not all nodes delivered (all optims)! aborting')
##                    print(ndelivers, "out of", n) 
##                    sys.exit()
##                else:
##                    print(' OK ', end='')
##                inf.close()
##
##                cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-allOptim'
##                cmd = 'rm stats/output_'+cname[1:]+'*'
##                out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
##                writeNedFile(G, n, k)
##                writeIniFile(n, k)
##                cmd='../BRB -r 0 -m -u Cmdenv -c '+cname+' -n ../src:.:../../../../../sim/inet4/src:../../../../../sim/inet4/examples:../../../../../sim/inet4/tutorials:../../../../../sim/inet4/showcases --image-path=../../../../../sim/inet4/images -l ../../../../../sim/inet4/src/INET '+ininame+' > /dev/null'
##                os.system(cmd)
##
##                ndelivers = 0
##                inf = open('stats/output_'+cname[1:]+'.delivers', 'r')
##                for line in inf:
##                    ndelivers = int(line.rstrip('\n'))
##                if ndelivers != n:
##                    print('Not all nodes delivered (no optims)! aborting')
##                    print(ndelivers, "out of", n)
##                    sys.exit()
##                else:
##                    print(' OK', end='')
##                    if delete_ini_ned_file:
##                        nedname = 'random_'+str(n)+'_'+str(k)+'.ned'
##                        cmd = 'rm '+ininame+' '+nedname
##                        out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
##                inf.close()
## 
##                cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-noOptim'
##                inf = open('stats/output_'+cname[1:]+'.msgs', 'r')
##                l0 = []
##                for line in inf:
##                    l0.append(float(line.rstrip('\n')))
##                inf.close()
##                l0_long.extend(l0)
##
##                cname='c'+str(n)+'_'+str(k)+'_'+str(f)+'-allOptim'
##                inf = open('stats/output_'+cname[1:]+'.msgs', 'r')
##                l1 = []
##                for line in inf:
##                    l1.append(float(line.rstrip('\n')))
##                inf.close()
##                l1_long.extend(l1)
##
##                msgsImprovement[n][f][k].append( (mean(l1)-mean(l0)) / mean(l0))
##                print('- msgs :'+str(int(100*(mean(l1)-mean(l0)) / mean(l0)))+'% ',end='')
##                print('('+str(mean(l1))+' '+str(mean(l0))+')')
##
##
##            resd[n][f][k][0] = l0_long
##            resd[n][f][k][1] = l1_long
                

                
                
                
