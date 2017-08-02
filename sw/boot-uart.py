#!/usr/bin/python3

our_port='/dev/ttyUSB1'
#  our_port='COM4'

import sys
import time
import serial
import os

ser = serial.Serial(port=our_port,baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,timeout=1)
print('Bootloader ready')
ser.reset_input_buffer()
ser.reset_output_buffer()

fr = open(sys.argv[1], 'rb')

while(ser.read(1) != b'B'):
	pass

while(ser.read(1) != b'o'):
	pass

while(ser.read(1) != b'o'):
	pass

while(ser.read(1) != b't'):
	pass

while(ser.read(1) != b'?'):
	pass

time.sleep(0.01)
print('Bootloader start')
ser.write(b'Y')

while(ser.read(1) != b'O'):
	pass

while(ser.read(1) != b'K'):
	pass

l = os.fstat(fr.fileno()).st_size
print('Bootloader OK, writing %d bytes.' % l)
len = l.to_bytes(4, byteorder='big')
ser.write(len)

n=0
while True:
	n = n+1
	b = fr.read(1)
	if n % 100 == 0:
		print("%d/%d bytes programmed." % (n,l))
	if (b == b''):
	    break
	else:
		ser.write(b)


while(ser.read(1) != b'G'):
	pass
while(ser.read(1) != b'o'):
	pass
while(ser.read(1) != b'!'):
	time.sleep(100e-6)

print('Programming done!')
ser.close()
fr.close()
