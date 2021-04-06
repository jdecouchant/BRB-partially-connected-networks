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

maxidconfig = 3
repeats = 5
nthreads = 64
msgSize = 128 # 1Kib, in bits


def printOmnetLinks(G, N):
    s = ''
    for i in range(N):
        for j in range(i, N):
            if G.has_edge(i, j):
                s += 'cli['+str(i)+'].pppg++ <--> ethernetline <--> cli['+str(j)+'].pppg++;\n'
    return s

def nedname(N, F, K):
    return 'random_'+str(N)+'_'+str(F)+'_'+str(K)+'_small_best.ned'

def ininame(N, F, K):
    return 'random_'+str(N)+'_'+str(F)+'_'+str(K)+'_small_best.ini'

def prefix(N, F, K):
    return str(N)+'_'+str(F)+'_'+str(K)+'_small_best'

def writeNedFile(G, N, F, K):

    # Generate the random.ned file
    fo = open(nedname(N,F,K), 'w')
    fo.write('package broadcastsign.simulations;\n\n'+\
            'import inet.common.misc.ThruputMeteringChannel;\n'+\
            'import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;\n'+\
            'import inet.node.inet.Router;\n'+\
            'import inet.node.inet.StandardHost;\n\n')
    fo.write('network random_'+prefix(N,F,K)+'\n\
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
    while not os.path.exists(nedname(N,F,K)):
        print('Ned file does not exist: waiting')
        time.sleep(1)

if os.path.exists('dic_small_best.txt'):
    subprocess.run('rm dic_small_best.txt', shell=True)            
if os.path.exists('msgsImprovement_small_best.txt'):
    subprocess.run('rm msgsImprovement_small_best.txt', shell=True)            
if os.path.exists('bitsImprovement_small_best.txt'):
    subprocess.run('rm bitsImprovement_small_best.txt', shell=True)            
if os.path.exists('latsImprovement_small_best.txt'):
    subprocess.run('rm latsImprovement_small_best.txt', shell=True)            
  
msgsImprovement = {}
bitsImprovement = {}
latsImprovement = {}
procImprovement = {}

#if os.path.isfile('dic.txt'):
#    inf = open("dic.txt", "rb")
#    resd = pickle.load(inf)
#    inf.close()

delete_ini_ned_file = True

subprocess.run('rm -r __pycache__/', shell=True)

lock = threading.Lock()

cmdsThreads = {}
prefixThreads = {}
cnameThreads = {}
ininameThreads = {}
GThreads = {}
argsThreads = {} # threadId -> n, f, k, configId

msgListThread = {}
bitListThread = {}
latListThread = {}
procListThread = {}

def runOneConfig():

    #print(threading.get_ident())
    success = False

    while not success:
        lock.acquire()
    #print(threading.get_ident(), ': entering')
        tid = threading.get_ident()
        n = argsThreads[tid][0]
        f = argsThreads[tid][1]
        k = argsThreads[tid][2]
        configId = argsThreads[tid][3]

        prefixThreads[tid] = prefix(n,f,k)
        cnameThreads[tid] ='c'+prefix(n,f,k)+'-'+str(configId)
        ininameThreads[tid] = ininame(n,f,k)

    #subprocess.run('rm stats/'+prefix(n,f,k)+'_*.graph', shell=True)
        subprocess.run('cp disjointPaths.py ./stats/'+prefix(n,f,k)+'.py', shell=True)
    
        writeNedFile(GThreads[tid], n, f, k)
    #writeIniFile(n, f, k)

        cmd='../BroadcastSign -m -u Cmdenv -c '+cnameThreads[tid]+\
        ' -n ../src:.:../../inet4/src:../../inet4/examples:../../inet4/tutorials:../../inet4/showcases --image-path=../../inet4/images -l ../../inet4/src/INET '+\
        ininameThreads[tid] + ' >> ./stats/log.'+cnameThreads[tid]
        cmdsThreads[tid] = cmd
    #print(cmd)
    #sys.exit()

    #inf = open('./stats/log.'+cnameThreads[tid], 'w')
    #inf.write(cmd)
    #inf.close()
    
    #print(threading.get_ident(), ': exiting')
        delivername = 'stats/'+prefixThreads[tid]+'.delivers'
        msgname = 'stats/'+prefixThreads[tid]+'.msgs'
        bitname = 'stats/'+prefixThreads[tid]+'.bits'
        latname = 'stats/'+prefixThreads[tid]+'.lat'
        procname = 'stats/'+prefixThreads[tid]+'.proc'

        if os.path.exists(delivername):
            subprocess.run('rm '+delivername, shell=True)            
        if os.path.exists(msgname):
            subprocess.run('rm '+msgname, shell=True)            
        if os.path.exists(bitname):
            subprocess.run('rm '+bitname, shell=True)            
        if os.path.exists(latname):
            subprocess.run('rm '+latname, shell=True)            
        if os.path.exists(procname):
            subprocess.run('rm '+procname, shell=True)

        lock.release()

    #print(cmdsThreads[threading.get_ident()])
        subprocess.run(cmdsThreads[threading.get_ident()], shell=True)

    #cmd='../BRB -r 0 -m -u Cmdenv -c '+cname+' -n ../src:.:../../../../../sim/inet4/src:../../../../../sim/inet4/examples:../../../../../sim/inet4/tutorials:../../../../../sim/inet4/showcases --image-path=../../../../../sim/inet4/images -l ../../../../../sim/inet4/src/INET '\
    # +ininame(n,f,k)+' > /dev/null'
    #print(cmd)
    #sys.exit()

    #print(cmdsThreads[threading.get_ident()])

        if (not os.path.exists('stats/'+prefixThreads[threading.get_ident()]+'.delivers')): 
        #or (not os.path.exists(msgname)) or (not os.path.exists(bitname)):
            print('Waiting for '+delivername+' and similar to exist')
            time.sleep(1)
            continue
        lock.acquire()
    
        tid = threading.get_ident()
        delivername = 'stats/'+prefixThreads[tid]+'.delivers'
        msgname = 'stats/'+prefixThreads[tid]+'.msgs'
        bitname = 'stats/'+prefixThreads[tid]+'.bits'
        latname = 'stats/'+prefixThreads[tid]+'.lat'
        procname = 'stats/'+prefixThreads[tid]+'.proc'

        n = argsThreads[tid][0]
        f = argsThreads[tid][1]
        k = argsThreads[tid][2]

        inf = open(delivername, 'r')
        ndelivers = 0
        for line in inf:
            ndelivers = int(line.rstrip('\n'))
        if ndelivers != n:
            print('Not all nodes delivered in '+delivername+'(no optim)! aborting')
            print("n =",n,", f =",f,", k =",k,'no optim')
            print(ndelivers, "out of", n)
        inf.close()

        inf = open(msgname, 'r')
        msgList = []
        for line in inf:
            msgList.append(float(line.rstrip('\n')))
        inf.close()

        inf = open(bitname, 'r')
        bitList = []
        for line in inf:
            bitList.append(float(line.rstrip('\n')))
        inf.close()

        inf = open(latname, 'r')
        latList = []
        for line in inf:
            latList.append(float(line.rstrip('\n')))
        inf.close()

        inf = open(procname, 'r')
        procList = []
        for line in inf:
            procList.append(float(line.rstrip('\n')))
        inf.close()
    
    #subprocess.run('rm stats/'+str(n)+'_'+str(f)+'_'+str(k)+'_small_best_*.graph', shell=True)            
        subprocess.run('rm '+delivername+' '+bitname+' '+latname+' '+msgname+' '+procname, shell=True)            
    
        msgListThread[tid] = msgList
        bitListThread[tid] = bitList
        latListThread[tid] = latList
        procListThread[tid] = procList
    
        lock.release()
        success = True

def runParams(nthreads):

    q = queue.Queue()
    for n in [31]: #range(10, 100, 6): # number of nodes
        msgsImprovement[n] = {}
        bitsImprovement[n] = {}
        latsImprovement[n] = {}
        procImprovement[n] = {}

        for f in [1,4,7,10,13,16]: #range(1, int((n-1)/3)+1, 2): # number of byzantine nodes
            msgsImprovement[n][f] = {}
            bitsImprovement[n][f] = {}
            latsImprovement[n][f] = {}
            procImprovement[n][f] = {}
            start = 2*f+1
            if (n * start % 2 == 1):
                start += 1
            for k in range(start, n, 2): #range(2*f+1, n-1): # graph connectivity

                if k*n % 2 == 1 or k < 2*f+1:
                    continue
                msgsImprovement[n][f][k] = {}
                bitsImprovement[n][f][k] = {}
                latsImprovement[n][f][k] = {}
                procImprovement[n][f][k] = {}
                for configId in range(maxidconfig+1):
                    msgsImprovement[n][f][k][configId] = []
                    bitsImprovement[n][f][k][configId] = []
                    latsImprovement[n][f][k][configId] = []
                    procImprovement[n][f][k][configId] = []
                q.put([n, f, k])


    def worker():
        
        while True:
            lock.acquire()
            config = q.get()
            
            if config is None:
                print('Thread is returning')
                lock.release()
                return
            
            n = config[0]
            f = config[1]
            k = config[2]
            tid = threading.get_ident()
            argsThreads[tid] = [n, f, k, 0]
            
            #subprocess.run('cp disjointPaths.py ./stats/'+prefix(n,f,k)+'.py', shell=True)
            lock.release()

            success = 0
            while success < repeats:
                # N*K must be even

                lock.acquire()
                tid = threading.get_ident()
                n = config[0]
                f = config[1]
                k = config[2]
                GThreads[tid] = generateGraphs.generate_KRegularKConnectedRandomGraph(n,k,f) # warning: arguments are reversed
                lock.release()

                for configId in range(maxidconfig+1):

                    lock.acquire()
                    tid = threading.get_ident()
                    argsThreads[tid][3] = configId
                    lock.release()

                    runOneConfig()
                    
                    lock.acquire()
           
                    tid = threading.get_ident()
                    n = argsThreads[tid][0]
                    f = argsThreads[tid][1]
                    k = argsThreads[tid][2]
                    configId = argsThreads[tid][3]

                    msgList = msgListThread[tid]
                    bitList = bitListThread[tid]
                    latList = latListThread[tid]
                    procList = procListThread[tid]

                    if len(msgList)>0 and len(bitList)>0 and len(latList)>0: 
                        msgsImprovement[n][f][k][configId].append(mean(msgList))
                        bitsImprovement[n][f][k][configId].append(mean(bitList))
                        latsImprovement[n][f][k][configId].append(mean(latList))
                        procImprovement[n][f][k][configId].append(mean(procList))
                        if configId != 0:
                            refMsg = msgsImprovement[n][f][k][0][-1]
                            refBit = bitsImprovement[n][f][k][0][-1]
                            refLat = latsImprovement[n][f][k][0][-1]
                            refProc = procImprovement[n][f][k][0][-1]
                        
                            print("cId="+str(configId)+", n ="+str(n)+", f="+str(f)+", k="+str(k)+', r='+str(success))
                            print('\t- msgs :'+str(100*(mean(msgList)-refMsg) / refMsg)+'% ',end='')
                            print('('+str(mean(msgList))+', '+str(refMsg)+')')
                            print('\t- bits :'+str(100*(mean(bitList)-refBit) / refBit)+'% ',end='')
                            print('('+str(mean(bitList))+', '+str(refBit)+')')
                            print('\t- latency :'+str(100*(mean(latList)-refLat) / refLat)+'% ',end='')
                            print('('+str(mean(latList))+', '+str(refLat)+')')
                            print('\t- proc :'+str(100*(mean(procList)-refProc) / refProc)+'% ',end='')
                            print('('+str(mean(procList))+', '+str(refProc)+')')
                   
                        #subprocess.run('cp disjointPaths.py ./stats/'+str(n)+'_'+str(f)+'_'+str(k)+'.py', shell=True)

                        outf = open("msgsImprovement_small_best.txt", "wb")
                        pickle.dump(msgsImprovement, outf)
                        outf.close()

                        outf = open("bitsImprovement_small_best.txt", "wb")
                        pickle.dump(bitsImprovement, outf)
                        outf.close()

                        outf = open("latsImprovement_small_best.txt", "wb")
                        pickle.dump(latsImprovement, outf)
                        outf.close()
                        
                        outf = open("procImprovement_small_best.txt", "wb")
                        pickle.dump(procImprovement, outf)
                        outf.close()
                        
                        lock.release()
                    else:
                        lock.release()

                success+=1

    threads = [ threading.Thread(target=worker) for _i in range(nthreads) ]
    for thread in threads:
        q.put(None)  # one EOF marker for each thread
        thread.start()

    for thread in threads:
        thread.join()



runParams(nthreads)






