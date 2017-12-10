import os

for j in [1,4]:
	for i in [5,8,12,511]:
		print "ram size: ",j,"GB and continuous count: ",i
		os.system("./simulate logs.txt "+str(j)+" 4096 "+str(i))
	
