import serial
import sys
import time

port = sys.argv[1]
baudrate = 9600

FIRST_BYTE='s'
LAST_BYTE='w'

constant_data = ''.join(chr(x) for x in [0x71, 0x72, 0x73, 0x74, 0x75, 0x76,0x77,0x78,0x79,0x60,0x61,0x62,0x63,0x64,0x65,0x66])

print "Using port: "+ port
conn = serial.Serial(port, baudrate)
if (not conn.isOpen()):
	conn.open()

def print16bytes(data):
	for val in data:
		print format(ord(val),"x") + " ",
	print ""

while True:
	if conn.inWaiting() > 0:
		startbyte = conn.read()
		if startbyte == FIRST_BYTE:
			controlbytes = conn.read(16)
			print "Receiving: ",
			print16bytes(controlbytes)
			startbyte = conn.read()
		if startbyte != LAST_BYTE:
			print 'invalid WAIT byte: '+ startbyte
			conn.flushInput()
		else :
			conn.write(FIRST_BYTE)
			print "Transmitting: ",
			print16bytes(constant_data)
			conn.write(constant_data)
		conn.write(LAST_BYTE)
	time.sleep(0.03)

