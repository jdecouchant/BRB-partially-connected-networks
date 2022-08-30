
fname = 'cpumem.txt'

f = open(fname, 'r')

maxcpu = {}
maxmem = {}

for n in [10, 30, 50]:
    maxcpu[n] = 0
    maxmem[n] = 0

n = 0
parity = 0
for l in f:
##    print(l)

    if parity == 0:
        l = l.rstrip('\n').split(' ')
        #print(l)
        n = int(l[2])
##        print(n)
    else:
        l = l.rstrip('\n').split('\t')
        #print(l)
        cpu = float(l[0].rstrip(' %'))
        mem = float(l[1].rstrip(' KB'))
##        print(cpu, mem)
        if cpu > maxcpu[n]:
            maxcpu[n] = cpu
        if mem > maxmem[n]:
            maxmem[n] = mem

    parity = (parity + 1) % 2

print(maxcpu, 'in % CPU')
print(maxmem, 'in KB')

f.close()
