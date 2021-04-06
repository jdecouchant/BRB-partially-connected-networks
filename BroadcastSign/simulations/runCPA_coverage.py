#!/usr/bin/python3

import sys
import random
import os
import subprocess
import pickle
import queue
import threading
import time


N = 10
k = 3
f = 1

nrepeats = 1

for N in [20]: #range(4, 100):
    f = 5 #int(N/3)
    if (3*f+1 > N):
        f+=1
    k = 2*f+1
    print('N:'+str(N)+', k:'+str(k)+', f:'+str(f))

    if N*k % 2 == 1:
        print('\t...skipping')
        continue

    os.system('rm stats/output_'+str(N)+'_'+str(k)+'_'+str(f)+'.delivers')

    start = time.time()
    for r in range(nrepeats):
        print('\t',r)
        os.system('python3 randomgraph.py '+str(N)+' '+str(k)+' '+str(f))

        sys.exit()
        os.system('../BRB -r 0 -m -u Cmdenv -c c'+str(N)+'_'+str(k)+'_'+str(f)+\
          ' -n ../src:.:../../../../../sim/inet4/src:../../../../../sim/inet4/examples:../../../../../sim/inet4/tutorials:../../../../../sim/inet4/showcases'+\
                  ' --image-path=../../../../../sim/inet4/images -l ../../../../../sim/inet4/src/INET '+\
                  'random.ini > /dev/null')
    end = time.time()
    print('\tDuration = ', end-start)
