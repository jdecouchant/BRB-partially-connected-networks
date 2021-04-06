#!/usr/bin/python3

import sys
import random
import os
import subprocess
import pickle
import queue
import threading

f_arr = [8, 16, 24, 66, 99, 133, 166, 333] #, 333] #8, 16, 24]#333]
T_over_d_arr = [8] #6,8]
pl_arr = [0] 
#pl_arr = [90,80,70,60,50,40,30,20,10,0]

nrepeats = 10

cmd = 'rm dic_reliability.txt'
out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

fo = open('reliability.ini', 'w')
fo.write('[General]\n')
fo.write('**.cmdenv-log-level = off\n')
fo.write('network = FullyConnected\n')
fo.write('simtime-resolution = ms\n')
fo.write('repeat = '+str(nrepeats)+'\n\n')

for f in f_arr:
    for td in T_over_d_arr:
        for pl in pl_arr:
            for X in [f+1, 2*f+1, 3*f]: #range(1,3*f+1+1,10): 
                fo.write('[Config c'+str(f)+'-'+str(td)+'-'+str(pl)+'-'+str(X)+']\n')
                fo.write('FullyConnected.mean             = 5ms\n')           # mean value for the channel delay
                fo.write('FullyConnected.stddev           = 0ms\n')             # standard deviation for the channel delay
                fo.write('FullyConnected.maxFaults        = '+str(f)+'\n')             # maximum number of faults
                fo.write('FullyConnected.p[*].maxFaults   = '+str(f)+'\n')               # ***same as above***
                fo.write('FullyConnected.p[*].timeout     = '+str(5*td)+'ms\n')           # timeout: here 100 milli-seconds
                fo.write('FullyConnected.p[*].maxDelay    = 5ms\n')            # expected maximum transimission delay: here 20ms
                fo.write('FullyConnected.p[*].numRand     = '+str(X)+'\n')               # number of (random) nodes to send to (at least f+1)
                fo.write('FullyConnected.p[*].numBcaster     = 1\n')
                fo.write('FullyConnected.p[*].bcastPeriod = 3\n')               # broadcast every bcastPeriod*timeout
                fo.write('FullyConnected.p[*].GC          = true\n')            # we garbage collect
                fo.write('FullyConnected.p[*].numBcast    = 1\n')               # total number of broadcast to do
                fo.write('FullyConnected.p[*].probaLosses = '+str(pl)+'\n')
                fo.write('FullyConnected.p[*].numPassive = 0\n')
                fo.write('FullyConnected.p[*].lossSend     = false\n')
                fo.write('FullyConnected.p[*].rotatingSend = true\n')
                fo.write('FullyConnected.p[*].numByz      = '+str(f)+'\n')
                fo.write('FullyConnected.p[*].durationOutput = "stats/duration_'+str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt"\n')
                fo.write('FullyConnected.p[*].passiveOutput = "stats/'+str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt"\n')
                fo.write('FullyConnected.p[*].recovery = false\n')
                fo.write('FullyConnected.p[*].passiveDuration = '+str(td)+'\n')
                fo.write('FullyConnected.p[*].sizeVal = 1\n')
                fo.write('FullyConnected.p[*].rtByzCastMode = false\n')
                fo.write('FullyConnected.p[*].endSimuDeliver = true\n')
                fo.write('FullyConnected.p[*].stopEarly = true\n')
                fo.write('\n')
fo.close()

print('Config file written!')
#sys.exit()

def runParams(f_arr, T_over_d_arr, pl_arr, num_threads):

    q = queue.Queue()
    l = []
    for f in f_arr:
        for td in T_over_d_arr:
            for pl in pl_arr:
                for X in [f+1, 2*f+1, 3*f]: #[167, 277, 387, 497]: # range(1,3*f+1+1,10):
                    #q.put([f, td, pl, X, 0])
                  #  q.put([f, td, pl, X, 1])
                    l = l + [[f, td, pl, X, 0]]
    #random.shuffle(l)
    for a in l: 
        q.put(a)

    lock = threading.Lock()

    resd = {}

    if os.path.isfile('dic_reliability.txt'):
        inf = open("dic_reliability.txt", "rb")
        resd = pickle.load(inf)
        inf.close()

    def worker():
      while True:
        config = q.get()
        if config is None:  # EOF?
          return

        f = config[0]
        td = config[1]
        pl = config[2]
        X = config[3]
        recovery = config[4]

        if f in resd and td in resd[f] and \
           pl in resd[f][td] and X in resd[f][td][pl] and \
           recovery in resd[f][td][pl][X]:
            continue

        print('./Pistis -m -u Cmdenv -c c'+\
              str(f)+'-'+str(td)+'-'+str(pl)+'-'+str(X)+'-'+str(recovery)+\
              ' reliability.ini > /dev/null')
        os.system('./Pistis -m -u Cmdenv -c c'+str(f)+'-'+str(td)+'-'+str(pl)+'-'+str(X)+' reliability.ini > /dev/null')
        #sys.exit()

        if os.path.isfile('stats/'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt'):
        
            crashList = []
            cf = open('stats/'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt', "r")
            nsuccess = 0
            for l in cf:
                ncrash = int(l)
                nalive = (3*f+1) - ncrash
                crashList.append(ncrash)
                if ncrash == f:
                    nsuccess += 1
            cf.close()
            if len(crashList) != nrepeats:
                print('CRASH WITH '+str(f)+'-'+str(td)+'-'+str(pl)+'-'+str(X))
            proba_shutdown = float(len(crashList)-nsuccess)/float(len(crashList))

            # name of variables is not correct
            crashList2 = []
            cf = open('stats/'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt.2f1', "r")
            nsuccess = 0
            for l in cf:
                ncrash = int(l)
                crashList2.append(ncrash)
                if ncrash == 1: ## Weird, why do we use == 1?
                    nsuccess += 1
            cf.close()
            if len(crashList2) != nrepeats:
                print('CRASH WITH '+str(f)+'-'+str(td)+'-'+str(pl)+'-'+str(X))
            proba_fails = float(len(crashList2)-nsuccess)/float(len(crashList2))
            
            #cmd = 'grep 0 stats/'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt | wc -l'
            #out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            #stdout,stderr = out.communicate()
            #nsuccess = int(stdout[:-1])
            #print(nsuccess)
            #proba_shutdown = float(nrepeats-nsuccess)/nrepeats
            #print(proba_shutdown)

            duration = -1
            durationMax = -1
            fullduration = -1
            dfname = 'stats/duration_'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt'
            if os.path.isfile(dfname):
                df = open(dfname, "r")
                durationList = []
                durationMaxList = []
                fulldurationlist = []
                c = 0
                for l in df:
                    #print(crashList[c])
                    if crashList[c] == f: # changed to f 
                        times = l.rstrip('\n').split(' ')
                        durationList.append(float(times[0]) + float(times[2]))
                        durationMaxList.append(float(times[0]) + float(times[1]))
                        fulldurationlist.append(float(times[3]))
                        c += 1
                df.close()
                if len(durationList) > 0:
                    duration = float(sum(durationList)) / float(len(durationList))
                    durationMax = float(sum(durationMaxList)) / float(len(durationMaxList))
                    fulldurationlist = float(sum(fulldurationlist)) / float(len(fulldurationlist))
                else:
                    duration = -1
                    durationMax = -1
                    fulldurationlist = -1
                
                #cmd = 'rm stats/duration_'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt'
                #out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            else:
                duration = -1
                durationMax = -1
                fulldurationlist = -1

            outbdw = -1
            bdwname = 'stats/'+ str(f)+'_'+str(td)+'_'+str(pl)+'_'+str(X)+'.txt.outbdw'
            if os.path.isfile(bdwname):
                bdw = open(bdwname, "r")
                bdwlist = []
                for l in bdw:
                    x = float(l.rstrip('\n'))
                    bdwlist.append(x)
                bdw.close()
            outbdw = float(sum(bdwlist))/float(len(bdwlist))

        else:
            proba_shutdown = 100.0
            duration = -1
            crashList = []
            proba_fails = -1 
            durationMax = -1
            outBdw = -1
            fullduration = -1

        lock.acquire()

        #print(resd)
        
        if not f in resd:
            resd[f] = {}
        if not td in resd[f]:
            resd[f][td] = {}
        if not pl in resd[f][td]:
            resd[f][td][pl] = {}
        if not X in resd[f][td][pl]:
            resd[f][td][pl][X] = {}
        resd[f][td][pl][X] = [proba_shutdown, duration, len(crashList), proba_fails, durationMax, outbdw, fulldurationlist]

        outf = open("dic_reliability.txt", "wb")
        pickle.dump(resd, outf)
        outf.close()

        lock.release() 

    nthreads = num_threads
    threads = [ threading.Thread(target=worker) for _i in range(nthreads) ]
    for thread in threads:
      q.put(None)  # one EOF marker for each thread
      thread.start()

    for thread in threads:
        thread.join()

#f_arr = [8, 16, 24] #, 166, 333]
#T_over_d_arr = [8]
#pl_arr = [0] #, 10, 20, 30, 40, 50, 60, 70, 80, 90]

#runParams([8,16,24], T_over_d_arr, pl_arr, 8) #f_arr, T_over_d_arr, pl_arr, num_threads
#runParams([66], T_over_d_arr, pl_arr, 8)
#runParams([99], T_over_d_arr, pl_arr, 4)
#runParams([133], T_over_d_arr, pl_arr, 4)
#runParams([166], T_over_d_arr, pl_arr, 4)
runParams([333], T_over_d_arr, pl_arr, 3) 
#runParams([8,16,24,66,99,133,166], T_over_d_arr, pl_arr, 1) 

