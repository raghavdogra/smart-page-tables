'''

ipfile = open('output.txt','r').readlines()
proc = []

for line in ipfile:
	line = line.split('\t')
	if line[0] != "PID MESSAGE\n":
		if int(line[0]) not in proc:
			proc.append(int(line[0]))



print len(proc)
'''

opfile = open('output2.txt','w')



for i in range(5):
	opfile.write(str(i) + '    A       1       '+str(0))
	opfile.write("\n")


for i in range(5):
	opfile.write(str(i) + '    A       1       '+str(2097152))
	opfile.write("\n")


for i in range(5):
	opfile.write(str(i) + '    A       1       '+str(4194304))
	opfile.write("\n")

opfile.close()