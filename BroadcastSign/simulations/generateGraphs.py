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

import kdiamond
import kpasted

def printOmnetLinks(G):
    for i in range(N):
        for j in range(i, N):
            if G.has_edge(i, j):
                print('cli['+str(i)+'].pppg++ <--> fiberline <--> cli['+str(j)+'].pppg++;');



def generate_KRegularKConnectedRandomGraph(N, k, f):
    # k-regular k-connected random graph (done)
    done = False
    while not done:
        G = nx.random_regular_graph(k, N)
        done = len(list(nx.connected_components(G))) == 1 and nx.node_connectivity(G) >= 2*f+1
    return G

def generate_generalizedWheel(N, k):
    # Generalized wheel (done)
    # Giovanni's code: https://github.com/giovannifarina/BFT-BRB/blob/master/generalized_wheel.py
    G = nx.Graph()
    for i in range(N):
        G.add_node(i)

    for i in range(N-k+2-1): # wheel of correct nodes 
        G.add_edge(i, i+1)
    G.add_edge(0, N-k+2-1)

    for i in range(N-k+2, N): # clique of faulty nodes
        for j in range(i+1, N):
            G.add_edge(i, j)

    for i in range(N-k+2): # connect correct node to all faulty 
        for j in range(N-k+2, N):
            G.add_edge(i, j)
    return G

def generate_multipartiteWheel(N, k):
    # Multipartite wheel
    # Giovanni's code: https://github.com/giovannifarina/BFT-BRB/blob/master/multipartite_wheel.py

    if k > N/2 or k%2 == 1:
        return None
    
    G = nx.Graph()
    for i in range(N):
        G.add_node(i)

        # disjoint groups of k/2 nodes
        # every node in group i is connected to all nodes of groups (i-1) and (i+1)
    gsize = k/2
    for gId in range(int(N/(k/2))-1):
        for nid1 in range(int(gId*(k/2)), int(gId*(k/2)+gsize)):
            for nid2 in range(int((gId+1)*(k/2)), int((gId+1)*(k/2)+gsize)):
                G.add_edge(nid1, nid2)

    gId = int(N/(k/2))-1
    for nid1 in range(int(0*(k/2)), int(0*(k/2)+gsize)):
        for nid2 in range(int(gId*(k/2)), int(gId*(k/2)+gsize)):
            G.add_edge(nid1, nid2)

    if N % (k/2) != 0:
        for i in range(int(N/(k/2)) * int(k/2), N):
            #print(i)
            for j in range(0, k):
                G.add_edge(i, j)
                #print(i, j)
            
    return G

def generate_kdiamond(N, k):
    G, nodes_to_analize = kdiamond.generate_k_diamond(N,k)
    return G

def generate_kpasted(N, k):
    G, Node_to_Analize = kpasted.generate_k_pasted(N, k)
    return G



