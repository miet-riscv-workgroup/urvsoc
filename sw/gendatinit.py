#!/usr/bin/python3
import sys 

in_f = open(sys.argv[1], 'rb')
out_f = open(sys.argv[2], 'w')
n=0
#while n<0x800:
#	dword00 = '00'
#	dword10 = '00'
#	dword20 = '00'
#	dword30 = '00'
#	out_f.write(dword30)
#	out_f.write(dword20)
#	out_f.write(dword10)
#	out_f.write(dword00)
#	out_f.write('\n')
#	n = n+4
	
while True:
	dword0 = in_f.read(1)
	dword1 = in_f.read(1)
	dword2 = in_f.read(1)
	dword3 = in_f.read(1)
	if (dword3 == b''):
	    break
	else:
		out_f.write(dword3.hex())
		out_f.write(dword2.hex())
		out_f.write(dword1.hex())
		out_f.write(dword0.hex())
		out_f.write('\n')
		n=n+4	
		
in_f.close()
out_f.close()
