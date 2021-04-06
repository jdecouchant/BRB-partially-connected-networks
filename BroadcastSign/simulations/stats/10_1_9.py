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

import networkx as nx

#print('basename', os.path.basename(__file__))
#print('dirname', os.path.dirname(__file__))
dirname = os.path.dirname(__file__)
basename = os.path.basename(__file__)

argfilename = dirname + '/' + basename.rstrip('.py') + '.args'
#print(argfilename)
argfile = open(argfilename, "r")
selfId = int(argfile.readline())
f = int(argfile.readline())
broadcasterId = int(argfile.readline())
msgType = int(argfile.readline())
argfile.close()

gfilename = dirname+'/'+basename.rstrip('.py')+'_'+\
            str(msgType)+"_"+str(broadcasterId)+"_"+str(selfId)+'.graph'
#print(gfilename)

gfile = open(gfilename, "r")
#gfile = open("/home/jeremie/sim/BroadcastSign/simulations/graph_"+str(msgType) \
#         +"_"+str(broadcasterId)+"_"+str(selfId)+".txt", "r")

t = []
lines = gfile.readlines()

gfile.close()
##print(lines)

numNodes = int(lines[0].strip())

G = nx.DiGraph()
pos = {}
for d in range(numNodes+1):
    for j in range(numNodes):
##        print(d*numNodes + j)
        G.add_node(d*numNodes + j)
        pos[d*numNodes+j] = (d*20, 1000-j*20)
##print(pos)

d = 0
for i in range(numNodes):
    iline = 2 + i*(numNodes+1)
    matrix = [[ 0 for i in range(numNodes)] for j in range(numNodes)]
    for src in range(numNodes):
        line = lines[iline+src].strip().rstrip(' ').split(' ')
##        print(src, line)
        for dst in range(len(line)):
            if int(line[dst]) == 1:
                matrix[src][dst] = 1
##    print(matrix)
    for src in range(numNodes):
        for dst in range(numNodes):
            if matrix[src][dst] == 1:
##                print('(',src, dst,')','(', d*numNodes+src, (d+1)*numNodes+dst,')')
                G.add_edge(d*numNodes+src, (d+1)*numNodes+dst)
    d += 1

maxDepth = d
##print(maxDepth*numNodes+localNode)

##return

resfilename = dirname+'/'+basename.rstrip('.py')+'.res'
file = open(resfilename, "w")
#file = open("/home/jeremie/sim/BroadcastSign/simulations/res.txt", "w")
##print(len(list(nx.node_disjoint_paths(G, 0, maxDepth*numNodes+selfId))))
found = False
for d in range(1, maxDepth+1): # no need to search in first layer
    if G.has_edge((d-1)*numNodes+broadcasterId, d*numNodes+selfId):
        found = True
        file.write('1')
        break
    try:
        #print("Search for disj. paths between 0 and ",d*numNodes+selfId)
        if len(list(nx.node_disjoint_paths(G, broadcasterId, d*numNodes+selfId))) > f:
            found = True
            file.write('1')
            break
    except:
        pass
if not found:
    file.write('0')
file.close()

#print(found)
#nx.draw(G, pos, with_labels=True)
#plt.show()

##plt.close()

##showGraph(G, pos)
##for line in f:
##    line = line.strip()
##    t += [int(x) for x in line.split(' ')]

##print(t)

##numNodes << f
##print(numNodes);
##



