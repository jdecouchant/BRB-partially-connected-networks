import sys

import numpy as np

from statistics import stdev
from statistics import median

from scipy.stats import f_oneway

color = sys.stdout.shell

#fname = 'async_table2.txt' # for async results 1024
#fname = 'table2.full.1024' # for sync results 1024

fname = 'sync_table2_16.txt' # for sync results 16

FILTER_ANOVA = False
ROUND_VAL = 2
option_cmp_to_mbd1 = True

f = open(fname, 'r')

c = 0

max_tops = 13
key = ''
top_msgs_options = {} #[]
top_lat_options = {} #[]
top_net_options = {} #[]

max_latency = 0 # to update
max_net = 0 # to update
max_msgs = 0 # to update

cur_options = ''

msgs = 0
lat = 0
net = 0

base_msgs = 0
base_lat = 0
base_net = 0

is_base = False
base_failed = False

is_mbd1 = False

for l in f:
    
    if 'False' in l or 'True' in l:
        cur_options = l.rstrip('\n')
        l = cur_options.split('\t')
        
        only_options = "\t".join(l[9:21])
        if only_options == "\t".join(['False']*12):
            is_base = True
        else:
            is_base = False
            
        if option_cmp_to_mbd1 and only_options == "True\t"+"\t".join(['False']*11):
            is_mbd1 = True
        else:
            is_mbd1 = False
        
        key = str(l[0])+' '+str(l[1])+' '+\
              str(l[2])+' '+str(l[3])
        
        if not key in top_msgs_options:
            top_msgs_options[key] = {}
            top_lat_options[key] = {}
            top_net_options[key] = {}
        if not cur_options in top_msgs_options[key]:
            top_msgs_options[key][cur_options] = []
            top_lat_options[key][cur_options] = []
            top_net_options[key][cur_options] = []

        
    if 'Num messages:' in l:
        l = l.rstrip('\n').split(' ')
        msgs = float(l[2])

        if is_base:
            base_msgs = msgs
            base_lat = lat
            base_net = net
        
        if lat != -1 and (not base_failed):
            top_msgs_options[key][cur_options].append([base_msgs, msgs])
            top_lat_options[key][cur_options].append([base_lat, lat])
            top_net_options[key][cur_options].append([base_net, net])

        if is_mbd1:
            base_msgs = msgs
            base_lat = lat
            base_net = net

    if 'Latency' in l:
         l = l.rstrip('\n').split(' ')
         lat = float(l[1])
         if is_base or is_mbd1:
##             base_lat = lat
             if lat == -1:
                 base_failed = True
             else:
                 base_failed = False
             
         
    if 'Net consumption' in l:
        l = l.rstrip('\n').split(' ')
        net = float(l[2])
##        if is_base or is_mbd1:
##            base_net = net

f.close()

def compare(key1, key2):
    
    s1 = key1.split(' ')
    s2 = key2.split(' ')
    if float(s1[0]) < float(s2[0]):
        return -1
    elif float(s1[0]) == float(s2[0]) and float(s1[1]) < float(s2[1]):
        return -1
    elif float(s1[0]) == float(s2[0]) and float(s1[1]) == float(s2[1]) and float(s1[2]) < float(s2[2]):
        return -1
    elif float(s1[0]) == float(s2[0]) and float(s1[1]) == float(s2[1]) and float(s1[2]) == float(s2[2]) and float(s1[3]) < float(s2[3]):
        return -1
    elif float(s1[0]) == float(s2[0]) and float(s1[1]) == float(s2[1]) and float(s1[2]) == float(s2[2]) and float(s1[3]) == float(s2[3]):
        return 0
    return 1

base_msgs = 0
base_net = 0
base_lat = 0

import functools

oneOnly = True
onlyMBD = 10

### Print results per configuration 

for k in sorted(top_msgs_options, key=functools.cmp_to_key(compare)):
    # k: 'N f k payloadSize'

    is_base = True
    s = k.split(' ')
    print('Configuration N='+s[0]+', f='+s[1]+', k='+s[2]+', payload='+s[3]+' (Msgs, Net, Lat)')
    cid = 0
    base_options = ''

    for cur_options in top_msgs_options[k]:

        if oneOnly and cid!=onlyMBD:
            cid += 1
            continue
        
        c = cur_options.split('\t')
        c = "\t".join(c[9:21])

        is_base = False
        if c == "\t".join(['False']*12):
            is_base = True

        l1 = []
        l2 = []
        l3 = []

        bl1 = []
        bl2 = []
        bl3 = []
        
        for i in range(len(top_msgs_options[k][cur_options])):
            l1.append(top_msgs_options[k][cur_options][i][1])
            l2.append(top_net_options[k][cur_options][i][1])
            l3.append(top_lat_options[k][cur_options][i][1])

            bl1.append(top_msgs_options[k][cur_options][i][0])
            bl2.append(top_net_options[k][cur_options][i][0])
            bl3.append(top_lat_options[k][cur_options][i][0])

        if len(l1) > 0:
            a0 = sum(l1)/len(l1)
            b0 = sum(l2)/len(l2)
            c0 = sum(l3)/len(l3)
        else:
            a0 = -1
            b0 = -1
            c0 = -1
        
        if is_base:
            base_options = cur_options
            base_msgs = a0
            base_net = b0
            base_lat = c0

        if len(l1) > 1:
            anova_msgs = f_oneway(bl1, l1).pvalue # reject if p-value lower than l1)>0:0.05 (i.e., significant difference)
            anova_net = f_oneway(bl2, l2).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
            anova_lat = f_oneway(bl3, l3).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
        else:
            anova_msgs = 1.0
            anova_net = 1.0
            anova_lat = 1.0

        dl1 = []
        dl2 = []
        dl3 = []
        for i in range(len(l1)):
            dl1.append(100 * (l1[i]-bl1[i])/bl1[i])
            dl2.append(100 * (l2[i]-bl2[i])/bl2[i])
            dl3.append(100 * (l3[i]-bl3[i])/bl3[i])

        if len(dl1) > 0:
            a = sum(dl1) / len(dl1) 
            b = sum(dl2) / len(dl2)
            c = sum(dl3) / len(dl3)
        else:
            a = -1
            b = -1
            c = -1
                       
        print('MBD.'+str(cid)+' ('+str(len(l1))+'pts)\t', end='')

        if FILTER_ANOVA and anova_msgs >= 0.05:
            print("()\t", end='')
        else:

            if len(l1) > 1:
                m = '('+str(round(a0, ROUND_VAL))+':'+str(round(a, ROUND_VAL))+'%, '+str(round(stdev(l1), ROUND_VAL))
            else:
                m = '('+str(round(a0, ROUND_VAL))+':'+str(round(a, ROUND_VAL))+'%, 0'
            col = 'KEYWORD'
            if not is_base:
                if a <= 0:
                    col = 'STRING'
                else:
                    col = 'COMMENT'
                color.write(m+',', col)
            else:
                print(m, end='')

            col = 'KEYWORD'
            if not is_base:
                if anova_msgs < 0.05:
                    col = 'STRING'
                else:
                    col = 'COMMENT'            
                color.write(str(round(anova_msgs, ROUND_VAL))+')\t', col)
            else:
                print(')\t', end='')

        if FILTER_ANOVA and anova_net >= 0.05:
            print("()\t", end='')
        else:
            if len(l2)>1:
                m = '('+str(round(b0, ROUND_VAL))+':'+str(round(b, ROUND_VAL))+'%, '+str(round(stdev(l2), ROUND_VAL))
            else:
                m = '('+str(round(b0, ROUND_VAL))+':'+str(round(b, ROUND_VAL))+'%, 0'
            col = 'KEYWORD'
            if not is_base:
                if b < 0:
                    col = 'STRING'
                else:
                    col = 'COMMENT'
                color.write(m+',', col)
            else:
                print(m, end='')
                
            col = 'KEYWORD'
            if not is_base:
                if anova_net < 0.05:
                    col = 'STRING'
                else:
                    col = 'COMMENT'
                color.write(str(round(anova_net, ROUND_VAL))+')\t', col)
            else:
                print(')\t', end='')

        if FILTER_ANOVA and anova_lat >= 0.05:
            print("()\t", end='')
        else:
            if len(l3)>1:
                m = '('+str(round(c0, ROUND_VAL))+':'+str(round(c, ROUND_VAL))+'%, '+str(round(stdev(l3), ROUND_VAL))
            else:
                m = '('+str(round(c0, ROUND_VAL))+':'+str(round(c, ROUND_VAL))+'%, 0'
            col = 'KEYWORD'
            if not is_base:
                if c < 0:
                    col = 'STRING'
                else:
                    col = 'COMMENT'
                color.write(m+',', col)
            else:
                print(m, end='')

            col = 'KEYWORD'
            if not is_base:
                if anova_lat < 0.05:
                    col = 'STRING'
                else:
                    col = 'COMMENT'
                color.write(str(round(anova_lat, ROUND_VAL))+')\t', col)
            else:
                print(')\t', end='')
                
        print()
                  
        cid += 1
    print()


statsPerMBD = {}
statsPerMBD['msg'] = {}
statsPerMBD['lat'] = {}
statsPerMBD['net'] = {}

for k in sorted(top_msgs_options, key=functools.cmp_to_key(compare)):
    # k: 'N f k payloadSize'
    
    is_base = True
    s = k.split(' ')
##    print('Configuration N='+s[0]+', f='+s[1]+', k='+s[2]+', payload='+s[3]+' (Msgs, Net, Lat)')
    cid = 0
    base_options = ''

    for cur_options in top_msgs_options[k]: # for all possible configurations
        c = cur_options.split('\t')
        c = "\t".join(c[9:21])

        if not cid in statsPerMBD['lat']:
            statsPerMBD['msg'][cid] = []
            statsPerMBD['lat'][cid] = []
            statsPerMBD['net'][cid] = []

        l1 = []
        l2 = []
        l3 = []

        bl1 = []
        bl2 = []
        bl3 = []
        
        for i in range(len(top_msgs_options[k][cur_options])):
            l1.append(top_msgs_options[k][cur_options][i][1])
            l2.append(top_net_options[k][cur_options][i][1])
            l3.append(top_lat_options[k][cur_options][i][1])

            bl1.append(top_msgs_options[k][cur_options][i][0])
            bl2.append(top_net_options[k][cur_options][i][0])
            bl3.append(top_lat_options[k][cur_options][i][0])

        for i in range(len(l1)):
            statsPerMBD['msg'][cid].append(100 * (l1[i]-bl1[i])/bl1[i])
            statsPerMBD['net'][cid].append(100 * (l2[i]-bl2[i])/bl2[i])
            statsPerMBD['lat'][cid].append(100 * (l3[i]-bl3[i])/bl3[i])  

        cid += 1

        
##print(statsPerMBD['msg'][1])
##print(statsPerMBD['net'][1])
##print(statsPerMBD['lat'][1])

# Print stats per modification
print('Modification Msgs Net Lat')
for cid in range(13):
    
    print('MBD.'+str(cid), end='\t')
    
##    avg = round(sum(statsPerMBD['msg'][cid]) / len(statsPerMBD['msg'][cid]), 1)

    l = statsPerMBD['msg'][cid]
    q1 = round(np.percentile(l, 25), 1)
    med = round(np.percentile(l, 50), 1)
    q3 = round(np.percentile(l, 75), 1)
    iqr = q3 - q1
    low = round(q1 - 1.5 * iqr, 1)
    high = round(q3 + 1.5 * iqr, 1)
    print('['+str(low)+', '+str(q1)+', '+str(med)+', '+str(q3)+', '+str(high)+']', end='\t')

    l = statsPerMBD['net'][cid]
    q1 = round(np.percentile(l, 25), 1)
    med = round(np.percentile(l, 50), 1)
    q3 = round(np.percentile(l, 75), 1)
    iqr = q3 - q1
    low = round(q1 - 1.5 * iqr, 1)
    high = round(q3 + 1.5 * iqr, 1)
    print('['+str(low)+', '+str(q1)+', '+str(med)+', '+str(q3)+', '+str(high)+']', end='\t')

    l = statsPerMBD['lat'][cid]
    q1 = round(np.percentile(l, 25), 1)
    med = round(np.percentile(l, 50), 1)
    q3 = round(np.percentile(l, 75), 1)
    iqr = q3 - q1
    low = round(q1 - 1.5 * iqr, 1)
    high = round(q3 + 1.5 * iqr, 1)
    print('['+str(low)+', '+str(q1)+', '+str(med)+', '+str(q3)+', '+str(high)+']')


print('\nMessages')
print('Mod low ql med qh high') 
for cid in range(1,13):
    print('MBD.'+str(cid), end=' ')
    
##    avg = round(sum(statsPerMBD['msg'][cid]) / len(statsPerMBD['msg'][cid]), 1)

    l = statsPerMBD['msg'][cid]
    q1 = round(np.percentile(l, 25), 1)
    med = round(np.percentile(l, 50), 1)
    q3 = round(np.percentile(l, 75), 1)
    iqr = q3 - q1
    low = round(q1 - 1.5 * iqr, 1)
    high = round(q3 + 1.5 * iqr, 1)
    print(str(low)+' '+str(q1)+' '+str(med)+' '+str(q3)+' '+str(high))

print('\nLatency')
print('Mod low ql med qh high') 
for cid in range(1,13):
    print('MBD.'+str(cid), end=' ')
    
##    avg = round(sum(statsPerMBD['msg'][cid]) / len(statsPerMBD['msg'][cid]), 1)

    l = statsPerMBD['lat'][cid]
    q1 = round(np.percentile(l, 25), 1)
    med = round(np.percentile(l, 50), 1)
    q3 = round(np.percentile(l, 75), 1)
    iqr = q3 - q1
    low = round(q1 - 1.5 * iqr, 1)
    high = round(q3 + 1.5 * iqr, 1)
    print(str(low)+' '+str(q1)+' '+str(med)+' '+str(q3)+' '+str(high))


print('\nNetwork')
print('Mod low ql med qh high') 
for cid in range(1,13):
    print('MBD.'+str(cid), end=' ')
    
##    avg = round(sum(statsPerMBD['msg'][cid]) / len(statsPerMBD['msg'][cid]), 1)

    l = statsPerMBD['net'][cid]
    q1 = round(np.percentile(l, 25), 1)
    med = round(np.percentile(l, 50), 1)
    q3 = round(np.percentile(l, 75), 1)
    iqr = q3 - q1
    low = round(q1 - 1.5 * iqr, 1)
    high = round(q3 + 1.5 * iqr, 1)
    print(str(low)+' '+str(q1)+' '+str(med)+' '+str(q3)+' '+str(high))

    
sys.exit()

















########################################
for target in range(1, 13):
    xdata = []
    ydata = []
    zdata = []

    for k in sorted(top_msgs_options, key=functools.cmp_to_key(compare)):

        is_base = True
        s = k.split(' ')
        cid = 0
        base_options = ''

        for cur_options in top_msgs_options[k]:
            c = cur_options.split('\t')
            c = "\t".join(c[9:21])

            is_base = False
            if c == "\t".join(['False']*12):
                is_base = True

            l1 = []
            l2 = []
            l3 = []

            bl1 = []
            bl2 = []
            bl3 = []
            
            for i in range(len(top_msgs_options[k][cur_options])):
                l1.append(top_msgs_options[k][cur_options][i][1])
                l2.append(top_net_options[k][cur_options][i][1])
                l3.append(top_lat_options[k][cur_options][i][1])

                bl1.append(top_msgs_options[k][cur_options][i][0])
                bl2.append(top_net_options[k][cur_options][i][0])
                bl3.append(top_lat_options[k][cur_options][i][0])

            if len(l1) > 0:
                a0 = sum(l1)/len(l1)
                b0 = sum(l2)/len(l2)
                c0 = sum(l3)/len(l3)
            else:
                a0 = -1
                b0 = -1
                c0 = -1
            
            if is_base:
                base_options = cur_options
                base_msgs = a0
                base_net = b0
                base_lat = c0

            if len(l1) > 1:
                anova_msgs = f_oneway(bl1, l1).pvalue # reject if p-value lower than l1)>0:0.05 (i.e., significant difference)
                anova_net = f_oneway(bl2, l2).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
                anova_lat = f_oneway(bl3, l3).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
            else:
                anova_msgs = 1.0
                anova_net = 1.0
                anova_lat = 1.0

            dl1 = []
            dl2 = []
            dl3 = []
            for i in range(len(l1)):
                dl1.append(100 * (l1[i]-bl1[i])/bl1[i]) # msgs
                dl2.append(100 * (l2[i]-bl2[i])/bl2[i]) # net
                dl3.append(100 * (l3[i]-bl3[i])/bl3[i]) # lat

            if len(dl1) > 0:
                a = sum(dl1) / len(dl1) 
                b = sum(dl2) / len(dl2)
                c = sum(dl3) / len(dl3)
            else:
                a = -1
                b = -1
                c = -1

            if cid == target:
                xdata.append(float(s[1]))
                ydata.append(float(s[2]))
                zdata.append(c)
                      
            cid += 1

##    import numpy as np
##    from mpl_toolkits import mplot3d
##    import matplotlib.pyplot as plt
##    plt.style.use('seaborn-poster')
##
##    fig = plt.figure()
##    ax = plt.axes(projection='3d')
##
##    # Set axes label
##    ax.set_xlabel('f',labelpad=20)
##    ax.set_xlim3d(0, 20)
##    ax.set_ylabel('K',labelpad=20)
##    ax.set_ylim3d(0, 50)
##    ax.set_zlabel('Lat',labelpad=20)

    print('\nOption Lat: MBD'+str(target))
    print(xdata)
    print(ydata)
    print(zdata)
    for i in range(len(xdata)):
        print('[', xdata[i],',', ydata[i],',', zdata[i], ']')

##    ax.scatter3D(xdata, ydata, zdata, c=zdata) # , cmap='Greens')
##    plt.show()

####################################

for target in range(1, 13):
    xdata = []
    ydata = []
    zdata = []

    for k in sorted(top_msgs_options, key=functools.cmp_to_key(compare)):

        is_base = True
        s = k.split(' ')
        cid = 0
        base_options = ''

        for cur_options in top_msgs_options[k]:
            c = cur_options.split('\t')
            c = "\t".join(c[9:21])

            is_base = False
            if c == "\t".join(['False']*12):
                is_base = True

            l1 = []
            l2 = []
            l3 = []

            bl1 = []
            bl2 = []
            bl3 = []
            
            for i in range(len(top_msgs_options[k][cur_options])):
                l1.append(top_msgs_options[k][cur_options][i][1])
                l2.append(top_net_options[k][cur_options][i][1])
                l3.append(top_lat_options[k][cur_options][i][1])

                bl1.append(top_msgs_options[k][cur_options][i][0])
                bl2.append(top_net_options[k][cur_options][i][0])
                bl3.append(top_lat_options[k][cur_options][i][0])

            if len(l1) > 0:
                a0 = sum(l1)/len(l1)
                b0 = sum(l2)/len(l2)
                c0 = sum(l3)/len(l3)
            else:
                a0 = -1
                b0 = -1
                c0 = -1
            
            if is_base:
                base_options = cur_options
                base_msgs = a0
                base_net = b0
                base_lat = c0

            if len(l1) > 1:
                anova_msgs = f_oneway(bl1, l1).pvalue # reject if p-value lower than l1)>0:0.05 (i.e., significant difference)
                anova_net = f_oneway(bl2, l2).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
                anova_lat = f_oneway(bl3, l3).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
            else:
                anova_msgs = 1.0
                anova_net = 1.0
                anova_lat = 1.0

            dl1 = []
            dl2 = []
            dl3 = []
            for i in range(len(l1)):
                dl1.append(100 * (l1[i]-bl1[i])/bl1[i]) # msgs
                dl2.append(100 * (l2[i]-bl2[i])/bl2[i]) # net
                dl3.append(100 * (l3[i]-bl3[i])/bl3[i]) # lat

            if len(dl1) > 0:
                a = sum(dl1) / len(dl1) 
                b = sum(dl2) / len(dl2)
                c = sum(dl3) / len(dl3)
            else:
                a = -1
                b = -1
                c = -1

            if cid == target:
                xdata.append(float(s[1]))
                ydata.append(float(s[2]))
                zdata.append(a)
                      
            cid += 1

##    import numpy as np
##    from mpl_toolkits import mplot3d
##    import matplotlib.pyplot as plt
##    plt.style.use('seaborn-poster')

##    fig = plt.figure()
##    ax = plt.axes(projection='3d')
##
##    # Set axes label
##    ax.set_xlabel('f',labelpad=20)
##    ax.set_xlim3d(0, 20)
##    ax.set_ylabel('K',labelpad=20)
##    ax.set_ylim3d(0, 50)
##    ax.set_zlabel('#Msg',labelpad=20)

    print('\nOption Msgs: MBD'+str(target))
    print(xdata)
    print(ydata)
    print(zdata)
    for i in range(len(xdata)):
        print('[', xdata[i],',', ydata[i],',', zdata[i], ']')

##    ax.scatter3D(xdata, ydata, zdata, c=zdata) # , cmap='Greens')
##    plt.show()

###############################################
for target in range(1, 13):
    xdata = []
    ydata = []
    zdata = []

    for k in sorted(top_msgs_options, key=functools.cmp_to_key(compare)):

        is_base = True
        s = k.split(' ')
        cid = 0
        base_options = ''

        for cur_options in top_msgs_options[k]:
            c = cur_options.split('\t')
            c = "\t".join(c[9:21])

            is_base = False
            if c == "\t".join(['False']*12):
                is_base = True

            l1 = []
            l2 = []
            l3 = []

            bl1 = []
            bl2 = []
            bl3 = []
            
            for i in range(len(top_msgs_options[k][cur_options])):
                l1.append(top_msgs_options[k][cur_options][i][1])
                l2.append(top_net_options[k][cur_options][i][1])
                l3.append(top_lat_options[k][cur_options][i][1])

                bl1.append(top_msgs_options[k][cur_options][i][0])
                bl2.append(top_net_options[k][cur_options][i][0])
                bl3.append(top_lat_options[k][cur_options][i][0])

            if len(l1) > 0:
                a0 = sum(l1)/len(l1)
                b0 = sum(l2)/len(l2)
                c0 = sum(l3)/len(l3)
            else:
                a0 = -1
                b0 = -1
                c0 = -1
            
            if is_base:
                base_options = cur_options
                base_msgs = a0
                base_net = b0
                base_lat = c0

            if len(l1) > 1:
                anova_msgs = f_oneway(bl1, l1).pvalue # reject if p-value lower than l1)>0:0.05 (i.e., significant difference)
                anova_net = f_oneway(bl2, l2).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
                anova_lat = f_oneway(bl3, l3).pvalue # reject if p-value lower than 0.05 (i.e., significant difference)
            else:
                anova_msgs = 1.0
                anova_net = 1.0
                anova_lat = 1.0

            dl1 = []
            dl2 = []
            dl3 = []
            for i in range(len(l1)):
                dl1.append(100 * (l1[i]-bl1[i])/bl1[i]) # msgs
                dl2.append(100 * (l2[i]-bl2[i])/bl2[i]) # net
                dl3.append(100 * (l3[i]-bl3[i])/bl3[i]) # lat

            if len(dl1) > 0:
                a = sum(dl1) / len(dl1) 
                b = sum(dl2) / len(dl2)
                c = sum(dl3) / len(dl3)
            else:
                a = -1
                b = -1
                c = -1

            if cid == target:
                xdata.append(float(s[1]))
                ydata.append(float(s[2]))
                zdata.append(b)
                      
            cid += 1

##    import numpy as np
##    from mpl_toolkits import mplot3d
##    import matplotlib.pyplot as plt
##    plt.style.use('seaborn-poster')
##
##    fig = plt.figure()
##    ax = plt.axes(projection='3d')
##
##    # Set axes label
##    ax.set_xlabel('f',labelpad=20)
##    ax.set_xlim3d(0, 20)
##    ax.set_ylabel('K',labelpad=20)
##    ax.set_ylim3d(0, 50)
##    ax.set_zlabel('Net',labelpad=20)

    print('\nOption Net: MBD'+str(target))
    print(xdata)
    print(ydata)
    print(zdata)
    for i in range(len(xdata)):
        print('[', xdata[i],',', ydata[i],',', zdata[i], ']')
##    ax.scatter3D(xdata, ydata, zdata, c=zdata) # , cmap='Greens')

##    plt.show()
