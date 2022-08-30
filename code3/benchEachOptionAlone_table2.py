import subprocess
import itertools
import os
import sys

numServers = 50
#numFaulty = 1

#connectivity = 3

payloadSize = 16 #1024 #16 # in Bytes

numBcasts = 5 # total num of broadcast per node
sleepTime = 2000000 # delay between 2 broadcasts in microsec
minBidMeasure = 0 # lowest broadcast id considered for stats PER NODE
maxBidMeasure = 500; # higher bound broadcast id for stats
experimentTime = 240 # in seconds
writingIntervals = 1

# 1 - Dolev, 2 - Bracha-Dolev, 3 - Opt Bracha-Dolev, 4 - Bracha-CPA, 5 - Opt Bracha-CPA
choice = 3

doPrintGraph = 1

MOD1 = 0
MOD2 = 0
MOD3 = 0
MOD4 = 0
MOD5 = 0

MBD_1 = 0 # Associate payload to ID to avoid resending + delete echo or ready sender with single hop
MBD_2 = 0 # Single-hop Send msgs // PB with this option
MBD_3 = 0 # Echo_Echo messages
MBD_4 = 0 # Ready_Echo messages
MBD_5 = 0 # Optimized Send messages 
MBD_6 = 0 # Ignore Echos rcvd after corresponding Ready delivered 
MBD_7 = 0 # Ignore all Echos rcvd after delivering content 
MBD_8 = 0 # Do not send Echos to Ready neighbor
MBD_9 = 0 # Do not send anything to neighbor that delivered 
MBD_10 = 0 # Ignore msgs whose path is a superpath of known path 
MBD_11 = 0 # Bracha overprovisioning - to improve in send_echo and send_ready 
MBD_12 = 0 # Source sends its new Echo to only 2f+1 other processes

asyncNet = 0

if not asyncNet:
    if payloadSize == 1024:
        resultsfilename = 'sync_table2.txt'
    elif payloadSize == 16:
        resultsfilename = 'sync_table2_16.txt'

print(resultsfilename)

if os.path.exists(resultsfilename):
    os.remove(resultsfilename)

def argsToCmd():
    return 'python3 testDockerLocalArgs.py '+str(numServers)+' '+str(numFaulty)+' '+str(connectivity)+' '+str(payloadSize)+' '+str(numBcasts)+' '+str(sleepTime)+' '+str(experimentTime)+' '+str(writingIntervals)+' '+str(MOD1)+' '+str(MOD2)+' '+str(MOD3)+' '+str(MOD4)+' '+str(MOD5)+' '+str(MBD_1)+' '+str(MBD_2)+' '+str(MBD_3)+' '+str(MBD_4)+' '+str(MBD_5)+' '+str(MBD_6)+' '+str(MBD_7)+' '+str(MBD_8)+' '+str(MBD_9)+' '+str(MBD_10)+' '+str(MBD_11)+' '+str(MBD_12)+' '+resultsfilename+' '+str(doPrintGraph)+' '+str(asyncNet)

def runCmd(cmd):
    print(cmd)

    process = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    for line in process.stdout:
        print(line.decode().rstrip('\n'))

    process.wait()

numRepeats = 20
for repeat in range(numRepeats):
    for numFaulty in [1, int(((numServers-1)/3+1)/2), int((numServers-1)/3)]:
        for connectivity in [2*numFaulty+1, int((2*numFaulty+1+numServers)/2), numServers-1]:

            MOD1 = 0
            MOD2 = 0
            MOD3 = 0
            MOD4 = 0
            MOD5 = 0
            
            MBD_1 = 0 # Associate payload to ID to avoid resending + delete echo or ready sender with single hop
            MBD_2 = 0 # Single-hop Send msgs // PB with this option
            MBD_3 = 0 # Echo_Echo messages
            MBD_4 = 0 # Ready_Echo messages
            MBD_5 = 0 # Optimized Send messages 
            MBD_6 = 0 # Ignore Echos rcvd after corresponding Ready delivered 
            MBD_7 = 0 # Ignore all Echos rcvd after delivering content 
            MBD_8 = 0 # Do not send Echos to Ready neighbor
            MBD_9 = 0 # Do not send anything to neighbor that delivered 
            MBD_10 = 0 # Ignore msgs whose path is a superpath of known path 
            MBD_11 = 0 # Bracha overprovisioning - to improve in send_echo and send_ready 
            MBD_12 = 0 # Source sends its new Echo to only 2f+1 other processes

            #doPrintGraph = 1
            #experimentTime = 360
            # baseline: no optim at all
            #cmd = argsToCmd()
            #runCmd(cmd)
            
            doPrintGraph = 1

            MOD1 = 1
            MOD2 = 1
            MOD3 = 1
            MOD4 = 1
            MOD5 = 1

            # Run one with only Bonomi's modificationss
            experimentTime = 80
            cmd = argsToCmd()
            runCmd(cmd)

            doPrintGraph = 0

            experimentTime = 30

            for x in range(1, 13):

                MBD_1 = 0  
                MBD_2 = 0 # Single-hop Send msgs // PB with this option
                MBD_3 = 0 # Echo_Echo messages
                MBD_4 = 0 # Ready_Echo messages
                MBD_5 = 0 # Optimized Send messages 
                MBD_6 = 0 # Ignore Echos rcvd after corresponding Ready delivered 
                MBD_7 = 0 # Ignore all Echos rcvd after delivering content 
                MBD_8 = 0 # Do not send Echos to Ready neighbor
                MBD_9 = 0 # Do not send anything to neighbor that delivered 
                MBD_10 = 0 # Ignore msgs whose path is a superpath of known path 
                MBD_11 = 0 # Bracha overprovisioning - to improve in send_echo and send_ready 
                MBD_12 = 0 # Source sends its new Echo to only 2f+1 other processes

                if x >= 1:
                    MBD_1 = 1

                if x == 1:
                    MBD_1 = 1
                elif x==2:
                    MBD_2 = 1
                elif x==3:
                    MBD_3 = 1
                elif x==4:
                    MBD_4 = 1
                elif x==5:
                    MBD_5 = 1
                elif x==6:
                    MBD_6 = 1
                elif x==7:
                    MBD_7 = 1
                elif x==8:
                    MBD_8 = 1
                elif x==9:
                    MBD_9 = 1
                elif x==10:
                    MBD_10 = 1
                elif x==11:
                    MBD_11 = 1
                elif x==12:
                    MBD_12 = 1
                
                cmd = argsToCmd()
                runCmd(cmd)
