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



for i in range(2):
	for j in range(0, 2097152, 4096):
		opfile.write(str(i) + '    A       1       '+str(j))
		opfile.write("\n")




opfile.close()