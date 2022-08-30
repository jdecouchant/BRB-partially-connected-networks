import sys

fname = 'results.txt.copy'

f = open(fname, 'r')

c = 0

max_tops = 10
key = ''
top_msgs_options = {} #[]
top_lat_options = {} #[]
top_net_options = {} #[]

max_latency = 0 # to update
max_net = 0 # to update
max_msgs = 0 # to update

cur_options = ''

for l in f:
    if 'False' in l or 'True' in l:
        cur_options = l.rstrip('\n')
        l = cur_options.split('\t')
        key = str(l[0])+' '+str(l[1])+' '+\
              str(l[2])+' '+str(l[3])
        if not key in top_msgs_options:
            top_msgs_options[key] = []
            top_lat_options[key] = []
            top_net_options[key] = []
        
    if 'Num messages:' in l:
        l = l.rstrip('\n').split(' ')
        msgs = float(l[2])

        if len(top_msgs_options[key]) < max_tops:
            top_msgs_options[key].append([cur_options, msgs])
        else:
            max_i = 0
            cur_max = 0
            for i in range(max_tops):
                if top_msgs_options[key][i][1] > cur_max:
                    cur_max = top_msgs_options[key][i][1]
                    max_i = i
            if msgs < top_msgs_options[key][max_i][1]:
                top_msgs_options[key][max_i] = [cur_options, msgs]
            
        if msgs > max_msgs:
            max_2_msgs = max_msgs
            max_msgs = float(l[2])
        elif msgs > max_2_msgs:
            max_2_msgs = float(l[2])

    if 'Latency' in l:
         l = l.rstrip('\n').split(' ')
         lat = float(l[1])

         if len(top_lat_options[key]) < max_tops:
             top_lat_options[key].append([cur_options, lat])
         else:
             max_i = 0
             cur_max = 0
             for i in range(max_tops):
                 if top_lat_options[key][i][1] > cur_max:
                     cur_max = top_lat_options[key][i][1]
                     max_i = i
             if lat < top_lat_options[key][max_i][1]:
                 top_lat_options[key][max_i] = [cur_options, lat]
            
         if lat > max_latency:
             max_2_latency = max_latency
             max_latency = lat
         elif lat > max_2_latency:
             max_2_latency = lat

    if 'Net consumption' in l:
        l = l.rstrip('\n').split(' ')
        net = float(l[2])

        if len(top_net_options[key]) < max_tops:
            top_net_options[key].append([cur_options, net])
        else:
            max_i = 0
            cur_max = 0
            for i in range(max_tops):
                if top_net_options[key][i][1] > cur_max:
                    cur_max = top_net_options[key][i][1]
                    max_i = i
            if net < top_net_options[key][max_i][1]:
                top_net_options[key][max_i] = [cur_options, net]
             
        if net > max_net:
            max_2_net = max_net
            max_net = lat
        elif net > max_2_net:
            max_2_net = net

f.close()

# sort 
for k in top_msgs_options:
    top_msgs_options[k].sort(key=lambda x: x[1])
    top_lat_options[k].sort(key=lambda x: x[1])
    top_net_options[k].sort(key=lambda x: x[1])

dic_config_to_id = {}
cid = 0
for k in top_msgs_options:
    for x in top_msgs_options[k]:
        c = x[0].rstrip('\n').split('\t')
        c = "\t".join(c[9:21])
        if not c in dic_config_to_id:
            dic_config_to_id[c] = str(cid)
            cid += 1
    for x in top_lat_options[k]:
        c = x[0].rstrip('\n').split('\t')
        c = "\t".join(c[9:21])
        if not c in dic_config_to_id:
            dic_config_to_id[c] = str(cid)
            cid += 1
    for x in top_net_options[k]:
        c = x[0].rstrip('\n').split('\t')
        c = "\t".join(c[9:21])
        if not c in dic_config_to_id:
            dic_config_to_id[c] = str(cid)
            cid += 1

    print('Top', max_tops, 'msgs configurations with', k)
    for x in top_msgs_options[k]:
        c = x[0].rstrip('\n').split('\t')
        c = "\t".join(c[9:21])
        print(dic_config_to_id[c]+"\t"+c+"\t"+str(x[1])+\
              '\t+'+str(round((float(x[1])-float(top_msgs_options[k][0][1])) / float(top_msgs_options[k][0][1]),1))+'%')
    print()

    print('Top', max_tops, 'net configurations with', k)
    for x in top_net_options[k]:
        c = x[0].rstrip('\n').split('\t')
        c = "\t".join(c[9:21])
        print(dic_config_to_id[c]+"\t"+c+"\t"+str(x[1])+\
              '\t+'+str(round((float(x[1])-float(top_net_options[k][0][1])) / float(top_net_options[k][0][1]),1))+'%')
    print()

    print('Top', max_tops, 'lat configurations with', k)
    for x in top_lat_options[k]:
        c = x[0].rstrip('\n').split('\t')
        c = "\t".join(c[9:21])
        print(dic_config_to_id[c]+"\t"+c+"\t"+str(x[1])+\
              '\t+'+str(round((float(x[1])-float(top_lat_options[k][0][1])) / float(top_lat_options[k][0][1]),1))+'%')
    print()

    proportion_success_msgs = [0] * 12
    proportion_success_lat = [0] * 12
    proportion_success_net = [0] * 12
    
    for j in range(max_tops):
        config = top_msgs_options[k][j][0].rstrip('\n').split('\t')
        for i in range(9, 21):
            if config[i] == 'True':
                proportion_success_msgs[i-9] += 1

    for j in range(max_tops):
        config = top_lat_options[k][j][0].rstrip('\n').split('\t')
        for i in range(9, 22):
            if config[i] == 'True':
                proportion_success_lat[i-9] += 1

    config = top_net_options[k][j][0].rstrip('\n').split('\t')
    for i in range(9, 22):
        if config[i] == 'True':
            proportion_success_net[i-9] += 1

    # compute how often each option participates in a config that improves a metric
    c = max_tops
    for i in range(len(proportion_success_msgs)):
        proportion_success_msgs[i] /= (float(c) / 100.0)
        proportion_success_lat[i] /= (float(c) / 100.0)
        proportion_success_net[i] /= (float(c) / 100.0)
        
        proportion_success_msgs[i] = round(proportion_success_msgs[i], 1) 
        proportion_success_lat[i] = round(proportion_success_lat[i], 1)
        proportion_success_net[i] = round(proportion_success_net[i], 1) 
    
    print(proportion_success_msgs)
    print(proportion_success_net)
    print(proportion_success_lat)
    print()

sys.exit()
      
print('Worst options for min #msgs:\n')
print(max_msgs, max_2_msgs)


print('Worst options for min lat:\n')
print(max_latency, max_2_latency)


print('Worst options for min network consumption:\n')
print(max_net, max_2_net)

fname = 'results.txt.copy'

base_msgs = 0
base_lat = 0
base_net = 0

f = open(fname, 'r')


is_base_config = False
config = ''

proportion_success_msgs = [0] * 12
proportion_success_lat = [0] * 12
proportion_success_net = [0] * 12


success_msgs_pair = {}

c = 0
for l in f:
    if 'False' in l or 'True' in l:
        c += 1
        config = l.rstrip('\n').split('\t')
##        if c == 2:
##            print(config)
            
    if 'Num messages:' in l:
        l = l.rstrip('\n').split(' ')
        msgs = float(l[2])
        if c == 2:
            base_msgs = msgs
        elif c > 2 and msgs < base_msgs:
            for i in range(9, 22):
                if config[i] == 'True':
                    proportion_success_msgs[i-9] += 1

            for i in range(9, 22):
                for j in range(i+1, 22):
                    if config[i] == 'True' and config[j] == 'True':
                        if (i, j) in success_msgs_pair:
                            success_msgs_pair[(i,j)] += 1
                        else:
                            success_msgs_pair[(i,j)] = 1
        
    if 'Latency' in l:
         l = l.rstrip('\n').split(' ')
         lat = float(l[1])
         if c == 2:
             base_lat = lat
         elif c > 2 and msgs < base_lat:
             for i in range(9, 22):
                 if config[i] == 'True':
                     proportion_success_lat[i-9] += 1

    if 'Net consumption' in l:
        l = l.rstrip('\n').split(' ')
        net = float(l[2])
        if c == 2:
            base_net = net 
        elif c > 2 and msgs < base_net:
            for i in range(9, 22):
                if config[i] == 'True':
                    proportion_success_net[i-9] += 1 
    
print('\nTotal number of configurations:', c, '\n')

for i in range(len(proportion_success_msgs)):
    proportion_success_msgs[i] /= (float(c) / 100.0)
    proportion_success_lat[i] /= (float(c) / 100.0)
    proportion_success_net[i] /= (float(c) / 100.0)
    
    proportion_success_msgs[i] = round(proportion_success_msgs[i], 1) 
    proportion_success_lat[i] = round(proportion_success_lat[i], 1)
    proportion_success_net[i] = round(proportion_success_net[i], 1) 
    
print(proportion_success_msgs)
print(proportion_success_lat)
print(proportion_success_net)

print(success_msgs_pair)



f.close()
