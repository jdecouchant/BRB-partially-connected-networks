import sys

import numpy as np

from statistics import stdev
from statistics import median

from scipy.stats import f_oneway

import matplotlib.pyplot as plt

color = sys.stdout.shell

fname = 'results_best_configs_fig7_8.txt'
##fname = 'table2.full.1024'

#### Generates the following figures:
#
# journal_fig8_a.pdf
# journal_fig7.pdf
# journal_fig8.pdf
# journal_fig9.pdf

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

cid = -1
for l in f:

    if 'False' in l or 'True' in l:

        cid = (cid + 1) % 4
        
        cur_options = l.rstrip('\n')
        l = cur_options.split('\t')
        
        only_options = "\t".join(l[9:21])
        
        key = str(l[0])+' '+str(l[1])+' '+\
              str(l[2])+' '+str(l[3])
        
        if not key in top_msgs_options:
            top_msgs_options[key] = {}
            top_lat_options[key] = {}
            top_net_options[key] = {}
            
        if not 'bdw' in top_msgs_options[key]:

            top_msgs_options[key]['base'] = []
            top_lat_options[key]['base'] = []
            top_net_options[key]['base'] = []
            
            top_msgs_options[key]['bdw'] = []
            top_lat_options[key]['bdw'] = []
            top_net_options[key]['bdw'] = []
            
            top_msgs_options[key]['lat'] = []
            top_lat_options[key]['lat'] = []
            top_net_options[key]['lat'] = []
            
            top_msgs_options[key]['bdwlat'] = []
            top_lat_options[key]['bdwlat'] = []
            top_net_options[key]['bdwlat'] = []
        
    if 'Num messages:' in l:
        l = l.rstrip('\n').split(' ')
        msgs = float(l[2])

        if cid == 0:
            top_msgs_options[key]['base'].append(msgs)
            top_lat_options[key]['base'].append(lat)
            top_net_options[key]['base'].append(net)

        elif cid == 1:
            top_msgs_options[key]['bdw'].append(msgs)
            top_lat_options[key]['bdw'].append(lat)
            top_net_options[key]['bdw'].append(net)

        elif cid == 2:
            top_msgs_options[key]['lat'].append(msgs)
            top_lat_options[key]['lat'].append(lat)
            top_net_options[key]['lat'].append(net)

        elif cid == 3:
            top_msgs_options[key]['bdwlat'].append(msgs)
            top_lat_options[key]['bdwlat'].append(lat)
            top_net_options[key]['bdwlat'].append(net)


    if 'Latency' in l:
         l = l.rstrip('\n').split(' ')
         lat = float(l[1])
             
         
    if 'Net consumption' in l:
        l = l.rstrip('\n').split(' ')
        net = float(l[2])
##        if is_base or is_mbd1:
##            base_net = net

f.close()

print(top_lat_options)
##print(top_net_options)

# TODO assemble values per baseline and plot depending on connectivity

karr = {}
karr[10] = [3, 6, 9]
karr[30] = [3, 9, 15, 21, 27]
karr[50] = [3, 9, 15, 21, 27, 33, 39, 45]


x = {}
y = {}
yavg = {}
nonAvgY = {}
numServers = 50
for f in [5]: #[1,5,9,13,16]
    x['base'] = []
    y['base'] = []
    x['bdw'] = []
    y['bdw'] = []
    x['lat'] = []
    y['lat'] = []
    x['bdwlat'] = []
    y['bdwlat'] = []

    yavg['base'] = []
    yavg['bdw'] = []
    yavg['lat'] = []
    yavg['bdwlat'] = []
    
    nonAvgY[f] = []
    for k in karr[numServers]: #[4, 10, 16, 22, 28, 34, 40, 46]:
        if k < 2*f + 1:
            continue
        x['base'].append(k)
        x['bdw'].append(k)
        x['lat'].append(k)
        x['bdwlat'].append(k)

        key = str(numServers)+' '+str(f)+' '+str(k)+' 1024'
        tmp = 0
        count = 0
        for a in top_lat_options[key]['base']:
            y['base'].append(a)
            tmp += a
            count += 1
        yavg['base'].append(tmp/count)

        tmp = 0
        count = 0
        for a in top_lat_options[key]['bdw']:    
            y['bdw'].append(a)
            tmp += a
            count += 1
        yavg['bdw'].append(tmp/count)
        
        tmp = 0
        count = 0
        for a in top_lat_options[key]['lat']:   
            y['lat'].append(a)
            tmp += a
            count += 1
        yavg['lat'].append(tmp/count)
        
        tmp = 0
        count = 0
        for a in top_lat_options[key]['bdwlat']:
            y['bdwlat'].append(a)
            tmp += a
            count += 1
        yavg['bdwlat'].append(tmp/count)

fig, ax = plt.subplots(figsize=(10,6))

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=20)
ax.tick_params(axis='both', which='minor', labelsize=20)

for i in range(len(yavg['lat'])):
    yavg['lat'][i] /= 1000
for i in range(len(yavg['bdw'])):
    yavg['bdw'][i] /= 1000
for i in range(len(yavg['bdwlat'])):
    yavg['bdwlat'][i] /= 1000
for i in range(len(yavg['base'])):
    yavg['base'][i] /= 1000
    
for i in range(len(y['lat'])):
    y['lat'][i] /= 1000
for i in range(len(y['bdw'])):
    y['bdw'][i] /= 1000
for i in range(len(y['bdwlat'])):
    y['bdwlat'][i] /= 1000
for i in range(len(y['base'])):
    y['base'][i] /= 1000

##ax.errorbar(x['lat'], yavg['lat'], yerr=np.std(y['lat']), marker='^', linestyle='dashed', linewidth=2, label='lat.')
##ax.errorbar(x['bdw'], yavg['bdw'], yerr=np.std(y['bdw']), marker='x', linestyle='dotted', linewidth=2, label='bdw.')
##ax.errorbar(x['bdwlat'], yavg['bdwlat'], yerr=np.std(y['bdwlat']), marker='+', linestyle='dashdot', linewidth=2, label='lat. & bdw.')
##ax.errorbar(x['base'], yavg['base'], yerr=np.std(y['base']), marker='o', linestyle='solid', linewidth=2, label='BDopt + MBD.1')
ax.errorbar(x['base'], yavg['base'], marker='o', linestyle='solid', linewidth=2, label='BDopt + MBD.1')
ax.errorbar(x['lat'], yavg['lat'], marker='^', linestyle='dashed', linewidth=2, label='Lat.')
ax.errorbar(x['bdw'], yavg['bdw'], marker='x', linestyle='dotted', linewidth=2, label='Bdw.')
ax.errorbar(x['bdwlat'], yavg['bdwlat'], marker='+', linestyle='dashdot', linewidth=2, label='Lat. & Bdw.')

ax.legend(prop={'size': 20})

ax.set_xlabel('Network connectivity', fontsize=20)
ax.set_ylabel('Latency (ms)', fontsize=20) #Variation latency (%)')
#ax.set_title('Line plot with error bars')

plt.xlim([0, 50])
plt.xticks([0,5,10,15,20,25,30,35,40,45,50])

#plt.show()
plt.savefig("journal_fig8_a.pdf", bbox_inches='tight')

#sys.exit()

x = {}
y = {}
yavg = {}
nonAvgY = {}
for f in [5]:
    x['base'] = []
    y['base'] = []
    x['bdw'] = []
    y['bdw'] = []
    x['lat'] = []
    y['lat'] = []
    x['bdwlat'] = []
    y['bdwlat'] = []

    yavg['base'] = []
    yavg['bdw'] = []
    yavg['lat'] = []
    yavg['bdwlat'] = []
    
    nonAvgY[f] = []
    for k in karr[numServers]: #[4, 10, 16, 22, 28, 34, 40, 46]:
        if k < 2*f + 1 or k >= numServers:
            continue
        x['base'].append(k)
        x['bdw'].append(k)
        x['lat'].append(k)
        x['bdwlat'].append(k)

        key = '50 '+str(f)+' '+str(k)+' 1024'
        tmp = 0
        count = 0
        for a in top_lat_options[key]['base']:
            y['base'].append(a)
            tmp += a
            count += 1
        yavg['base'].append(tmp/count)
        avgbase = tmp/count

        tmp = 0
        count = 0
        for a in top_lat_options[key]['bdw']:    
            y['bdw'].append((a-avgbase)*100/avgbase)
            tmp += a
            count += 1
        yavg['bdw'].append( (tmp/count-avgbase) * 100/avgbase )
        
        tmp = 0
        count = 0
        for a in top_lat_options[key]['lat']:   
            y['lat'].append((a-avgbase)*100/avgbase)
            tmp += a
            count += 1
        yavg['lat'].append((tmp/count-avgbase) * 100/avgbase )
        
        tmp = 0
        count = 0
        for a in top_lat_options[key]['bdwlat']:
            y['bdwlat'].append((a-avgbase)*100/avgbase)
            tmp += a
            count += 1
        yavg['bdwlat'].append((tmp/count-avgbase) * 100/avgbase )

fig, ax = plt.subplots(figsize=(10,6))

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=20)
ax.tick_params(axis='both', which='minor', labelsize=20)

##ax.errorbar(x['base'], yavg['base'], yerr=np.std(y['base']), marker='o', linestyle='solid', label='Base config')

##ax.errorbar(x['lat'], yavg['lat'], yerr=np.std(y['lat']), marker='^', linestyle='dashed', linewidth=2, label='lat.')
##ax.errorbar(x['bdw'], yavg['bdw'], yerr=np.std(y['bdw']), marker='x', linestyle='dotted', linewidth=2, label='bdw')
##ax.errorbar(x['bdwlat'], yavg['bdwlat'], yerr=np.std(y['bdwlat']), marker='+', linestyle='dashdot', linewidth=2, label='lat. & bdw.')
ax.errorbar(x['lat'], yavg['lat'], marker='^', linestyle='dashed', linewidth=2, label='Lat.')
ax.errorbar(x['bdw'], yavg['bdw'], marker='x', linestyle='dotted', linewidth=2, label='Bdw')
ax.errorbar(x['bdwlat'], yavg['bdwlat'], marker='+', linestyle='dashdot', linewidth=2, label='Lat. & Bdw.')

ax.legend(prop={'size': 20})

ax.set_xlabel('Network connectivity', fontsize=20)
ax.set_ylabel('Variation latency (%)', fontsize=20)
#ax.set_title('Line plot with error bars')

plt.xlim([0, 50])
plt.xticks([0,5,10,15,20,25,30,35,40,45,50])

#plt.show()
plt.savefig("journal_fig7.pdf", bbox_inches='tight')

x = {}
y = {}
yavg = {}
nonAvgY = {}
numServers = 50
for f in [int(numServers/10)]:
    x['base'] = []
    y['base'] = []
    x['bdw'] = []
    y['bdw'] = []
    x['lat'] = []
    y['lat'] = []
    x['bdwlat'] = []
    y['bdwlat'] = []

    yavg['base'] = []
    yavg['bdw'] = []
    yavg['lat'] = []
    yavg['bdwlat'] = []
    
    nonAvgY[f] = []
    for k in karr[numServers]: #[4, 10, 16, 22, 28, 34, 40, 46]:
        if k < 2*f + 1 or k >= numServers:
            continue
        x['base'].append(k)
        x['bdw'].append(k)
        x['lat'].append(k)
        x['bdwlat'].append(k)

        key = str(numServers)+' '+str(f)+' '+str(k)+' 1024'
        tmp = 0
        count = 0
        for a in top_net_options[key]['base']:
            y['base'].append(a)
            tmp += a
            count += 1
        yavg['base'].append(tmp/count)

        tmp = 0
        count = 0
        for a in top_net_options[key]['bdw']:    
            y['bdw'].append(a)
            tmp += a
            count += 1
        yavg['bdw'].append(tmp/count)
        
        tmp = 0
        count = 0
        for a in top_net_options[key]['lat']:   
            y['lat'].append(a)
            tmp += a
            count += 1
        yavg['lat'].append(tmp/count)
        
        tmp = 0
        count = 0
        for a in top_net_options[key]['bdwlat']:
            y['bdwlat'].append(a)
            tmp += a
            count += 1
        yavg['bdwlat'].append(tmp/count)

fig, ax = plt.subplots(figsize=(10,6))

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=20)
ax.tick_params(axis='both', which='minor', labelsize=20)

for i in range(len(yavg['lat'])):
    yavg['lat'][i] /= 1000
for i in range(len(yavg['bdw'])):
    yavg['bdw'][i] /= 1000
for i in range(len(yavg['bdwlat'])):
    yavg['bdwlat'][i] /= 1000
for i in range(len(yavg['base'])):
    yavg['base'][i] /= 1000
    
for i in range(len(y['lat'])):
    y['lat'][i] /= 1000
for i in range(len(y['bdw'])):
    y['bdw'][i] /= 1000
for i in range(len(y['bdwlat'])):
    y['bdwlat'][i] /= 1000
for i in range(len(y['base'])):
    y['base'][i] /= 1000

##ax.errorbar(x['lat'], yavg['lat'], yerr=np.std(y['lat']), marker='^', linestyle='dashed', linewidth=2, label='lat.')
##ax.errorbar(x['bdw'], yavg['bdw'], yerr=np.std(y['bdw']), marker='x', linestyle='dotted', linewidth=2, label='bdw.')
##ax.errorbar(x['bdwlat'], yavg['bdwlat'], yerr=np.std(y['bdwlat']), marker='+', linestyle='dashdot', linewidth=2, label='lat. & bdw.')
##ax.errorbar(x['base'], yavg['base'], yerr=np.std(y['base']), marker='o', linestyle='solid', linewidth=2, label='BDopt + MBD.1')
ax.errorbar(x['base'], yavg['base'], marker='o', linestyle='solid', linewidth=2, label='BDopt + MBD.1')
ax.errorbar(x['lat'], yavg['lat'], marker='^', linestyle='dashed', linewidth=2, label='Lat.')
ax.errorbar(x['bdw'], yavg['bdw'], marker='x', linestyle='dotted', linewidth=2, label='Bdw.')
ax.errorbar(x['bdwlat'], yavg['bdwlat'], marker='+', linestyle='dashdot', linewidth=2, label='Lat. & Bdw.')

ax.set_xlabel('Network connectivity', fontsize=20)
ax.set_ylabel('Network consumption (kB)', fontsize=20) #Variation latency (%)')
#ax.set_title('Line plot with error bars')

plt.ylim([0, 200])
plt.xlim([0, 50])
plt.xticks([0,5,10,15,20,25,30,35,40,45,50])

ax.legend(prop={'size': 20}, loc='lower left', ncol=2)

#plt.show()
plt.savefig("journal_fig8.pdf", bbox_inches='tight')


x = {}
y = {}
yavg = {}
nonAvgY = {}
numServers = 50
for f in [5]:
    x['base'] = []
    y['base'] = []
    x['bdw'] = []
    y['bdw'] = []
    x['lat'] = []
    y['lat'] = []
    x['bdwlat'] = []
    y['bdwlat'] = []

    yavg['base'] = []
    yavg['bdw'] = []
    yavg['lat'] = []
    yavg['bdwlat'] = []
    
    nonAvgY[f] = []
    for k in karr[numServers]: #range(3, numServers+1, 6): #[4, 10, 16, 22, 28, 34, 40, 46]:
        if k < 2*f + 1 or k >= numServers:
            continue
        x['base'].append(k)
        x['bdw'].append(k)
        x['lat'].append(k)
        x['bdwlat'].append(k)

        key = str(numServers)+' '+str(f)+' '+str(k)+' 1024'
        tmp = 0
        count = 0
        for a in top_net_options[key]['base']:
            y['base'].append(a)
            tmp += a
            count += 1
        yavg['base'].append(tmp/count)
        avgbase = tmp/count

        tmp = 0
        count = 0
        for a in top_net_options[key]['bdw']:    
            y['bdw'].append((a-avgbase)*100/avgbase)
            tmp += a
            count += 1
        yavg['bdw'].append( (tmp/count-avgbase) * 100/avgbase)
        
        tmp = 0
        count = 0
        for a in top_net_options[key]['lat']:   
            y['lat'].append((a-avgbase)*100/avgbase)
            tmp += a
            count += 1
        yavg['lat'].append((tmp/count - avgbase) * 100/avgbase)
        
        tmp = 0
        count = 0
        for a in top_net_options[key]['bdwlat']:
            y['bdwlat'].append((a-avgbase)*100/avgbase)
            tmp += a
            count += 1
        yavg['bdwlat'].append((tmp/count-avgbase) * 100/avgbase)

fig, ax = plt.subplots(figsize=(10,6))

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=20)
ax.tick_params(axis='both', which='minor', labelsize=20)

# ax.errorbar(x['base'], yavg['base'], yerr=np.std(y['base']), marker='o', linestyle='solid', label='Base config')
##ax.errorbar(x['lat'], yavg['lat'], yerr=np.std(y['lat']), marker='^', linestyle='dashed', linewidth=2, label='lat.')
##ax.errorbar(x['bdw'], yavg['bdw'], yerr=np.std(y['bdw']), marker='x', linestyle='dotted', linewidth=2, label='bdw.')
##ax.errorbar(x['bdwlat'], yavg['bdwlat'], yerr=np.std(y['bdwlat']), marker='+', linestyle='dashdot', linewidth=2, label='lat. & bdw.')
ax.errorbar(x['lat'], yavg['lat'], marker='^', linestyle='dashed', linewidth=2, label='Lat.')
ax.errorbar(x['bdw'], yavg['bdw'], marker='x', linestyle='dotted', linewidth=2, label='Bdw.')
ax.errorbar(x['bdwlat'], yavg['bdwlat'], marker='+', linestyle='dashdot', linewidth=2, label='Lat. & Bdw.')

ax.legend(prop={'size': 20})

ax.set_xlabel('Network connectivity', fontsize=20)
ax.set_ylabel('Variation network consumption (%)', fontsize=20)
# ax.set_title('Line plot with error bars')

plt.ylim([-70, 0])
plt.xlim([0, 50])
plt.xticks([0,5,10,15,20,25,30,35,40,45,50])

# plt.show()
plt.savefig("journal_fig9.pdf", bbox_inches='tight')

x = {}
y = {}
yavg = {}
nonAvgY = {}
for numServers in [10, 30, 50]:

    x[numServers] = {}
    y[numServers] = {}
    yavg[numServers] = {}

    x[numServers]['base'] = []
    y[numServers]['base'] = []
    x[numServers]['bdw'] = []
    y[numServers]['bdw'] = []
    x[numServers]['lat'] = []
    y[numServers]['lat'] = []
    x[numServers]['bdwlat'] = []
    y[numServers]['bdwlat'] = []

    yavg[numServers]['base'] = []
    yavg[numServers]['bdw'] = []
    yavg[numServers]['lat'] = []
    yavg[numServers]['bdwlat'] = []
    
    for f in [5]: #[1,5,9,13,16]:

        if numServers < 3*f+1: # not enough nodes
            continue
        
        nonAvgY[f] = []
        for k in karr[numServers]: #[4, 10, 16, 22, 28, 34, 40, 46]:
            if k < 2*f + 1 or k >= numServers:
                continue
    
            x[numServers]['base'].append(k)
            x[numServers]['bdw'].append(k)
            x[numServers]['lat'].append(k)
            x[numServers]['bdwlat'].append(k)

            key = str(numServers)+' '+str(f)+' '+str(k)+' 1024'
            tmp = 0
            count = 0
            for a in top_net_options[key]['base']:
                y[numServers]['base'].append(a)
                tmp += a
                count += 1
            yavg[numServers]['base'].append(tmp/count)
            avgbase = tmp/count

            tmp = 0
            count = 0
            for a in top_net_options[key]['bdw']:    
                y[numServers]['bdw'].append((a-avgbase)*100/avgbase)
                tmp += a
                count += 1
            yavg[numServers]['bdw'].append( (tmp/count-avgbase) * 100/avgbase)
            
            tmp = 0
            count = 0
            for a in top_net_options[key]['lat']:   
                y[numServers]['lat'].append((a-avgbase)*100/avgbase)
                tmp += a
                count += 1
            yavg[numServers]['lat'].append((tmp/count - avgbase) * 100/avgbase)
            
            tmp = 0
            count = 0
            for a in top_net_options[key]['bdwlat']:
                y[numServers]['bdwlat'].append((a-avgbase)*100/avgbase)
                tmp += a
                count += 1
            yavg[numServers]['bdwlat'].append((tmp/count-avgbase) * 100/avgbase)

fig, ax = plt.subplots(figsize=(10,6))

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=20)
ax.tick_params(axis='both', which='minor', labelsize=20)

# ax.errorbar(x['base'], yavg['base'], yerr=np.std(y['base']), marker='o', linestyle='solid', label='Base config')
colors = ['blue', 'red', 'green']
i = -1
for numServers in [30,50]: #, 30, 50]:
    i += 1
    ax.errorbar(x[numServers]['lat'], yavg[numServers]['lat'], color = colors[i], marker='^', linestyle='dashed', linewidth=2, label='Lat., N='+str(numServers))
    ax.errorbar(x[numServers]['bdw'], yavg[numServers]['bdw'], color = colors[i], marker='x', linestyle='dotted', linewidth=2, label='Bdw., N='+str(numServers))
##    ax.errorbar(x[numServers]['lat'], yavg[numServers]['lat'], yerr=np.std(y[numServers]['lat']), marker='^', linestyle='dashed', linewidth=2, label='lat.'+str(numServers))
##    ax.errorbar(x[numServers]['bdw'], yavg[numServers]['bdw'], yerr=np.std(y[numServers]['bdw']), marker='x', linestyle='dotted', linewidth=2, label='bdw.'+str(numServers))
##ax.errorbar(x['bdwlat'], yavg['bdwlat'], yerr=np.std(y['bdwlat']), marker='+', linestyle='dashdot', linewidth=2, label='lat. & bdw.')

print(x[30]['lat'], yavg[30]['lat'])

ax.legend(prop={'size': 20})

ax.set_xlabel('Network connectivity', fontsize=20)
ax.set_ylabel('Variation network consumption (%)', fontsize=20)
# ax.set_title('Line plot with error bars')

plt.ylim([-70, 0])
plt.xlim([0, 50])
plt.xticks([0,5,10,15,20,25,30,35,40,45,50])

# plt.show()
plt.savefig("journal_fig10.pdf", bbox_inches='tight')


x = {}
y = {}
yavg = {}
nonAvgY = {}
for numServers in [10, 30, 50]:

    x[numServers] = {}
    y[numServers] = {}

    yavg[numServers] = {}

    x[numServers]['base'] = []
    y[numServers]['base'] = []
    x[numServers]['bdw'] = []
    y[numServers]['bdw'] = []
    x[numServers]['lat'] = []
    y[numServers]['lat'] = []
    x[numServers]['bdwlat'] = []
    y[numServers]['bdwlat'] = []

    yavg[numServers]['base'] = []
    yavg[numServers]['bdw'] = []
    yavg[numServers]['lat'] = []
    yavg[numServers]['bdwlat'] = []
    
    for f in [5]: #[1,5,9,13,16]:

        if numServers < 3*f+1:
            continue
        
        nonAvgY[f] = []
        for k in karr[numServers]: #[4, 10, 16, 22, 28, 34, 40, 46]:
            if k < 2*f + 1 or k >= numServers:
                continue
            x[numServers]['base'].append(k)
            x[numServers]['bdw'].append(k)
            x[numServers]['lat'].append(k)
            x[numServers]['bdwlat'].append(k)

            key = str(numServers)+' '+str(f)+' '+str(k)+' 1024'
            tmp = 0
            count = 0
            for a in top_lat_options[key]['base']:
                y[numServers]['base'].append(a)
                tmp += a
                count += 1
            yavg[numServers]['base'].append(tmp/count)
            avgbase = tmp/count

            tmp = 0
            count = 0
            for a in top_lat_options[key]['bdw']:    
                y[numServers]['bdw'].append((a-avgbase)*100/avgbase)
                tmp += a
                count += 1
            yavg[numServers]['bdw'].append( (tmp/count-avgbase) * 100/avgbase)
            
            tmp = 0
            count = 0
            for a in top_lat_options[key]['lat']:   
                y[numServers]['lat'].append((a-avgbase)*100/avgbase)
                tmp += a
                count += 1
            yavg[numServers]['lat'].append((tmp/count - avgbase) * 100/avgbase)
            
            tmp = 0
            count = 0
            for a in top_lat_options[key]['bdwlat']:
                y[numServers]['bdwlat'].append((a-avgbase)*100/avgbase)
                tmp += a
                count += 1
            yavg[numServers]['bdwlat'].append((tmp/count-avgbase) * 100/avgbase)

fig, ax = plt.subplots(figsize=(10,6))

# We change the fontsize of minor ticks label 
ax.tick_params(axis='both', which='major', labelsize=20)
ax.tick_params(axis='both', which='minor', labelsize=20)

# ax.errorbar(x['base'], yavg['base'], yerr=np.std(y['base']), marker='o', linestyle='solid', label='Base config')
colors = ['blue', 'red', 'green']
i = -1
for numServers in [30, 50]: # 10 too large
    i += 1
    ax.errorbar(x[numServers]['lat'], yavg[numServers]['lat'], color = colors[i], marker='^', linestyle='dashed', linewidth=2, label='Lat., N='+str(numServers))
    ax.errorbar(x[numServers]['bdw'], yavg[numServers]['bdw'], color = colors[i], marker='x', linestyle='dotted', linewidth=2, label='Bdw., N='+str(numServers))
##    ax.errorbar(x[numServers]['lat'], yavg[numServers]['lat'], yerr=np.std(y[numServers]['lat']), marker='^', linestyle='dashed', linewidth=2, label='lat.'+str(numServers))
##    ax.errorbar(x[numServers]['bdw'], yavg[numServers]['bdw'], yerr=np.std(y[numServers]['bdw']), marker='x', linestyle='dotted', linewidth=2, label='bdw.'+str(numServers))
##ax.errorbar(x['bdwlat'], yavg['bdwlat'], yerr=np.std(y['bdwlat']), marker='+', linestyle='dashdot', linewidth=2, label='lat. & bdw.')

ax.legend(prop={'size': 20})

ax.set_xlabel('Network connectivity', fontsize=20)
ax.set_ylabel('Variation latency (%)', fontsize=20)
# ax.set_title('Line plot with error bars')

plt.ylim([-10, 20])
plt.xlim([0, 50])
plt.xticks([0,5,10,15,20,25,30,35,40,45,50])

# plt.show()
plt.savefig("journal_fig11.pdf", bbox_inches='tight')
