import serial
import sys
import time

port = sys.argv[1]
baudrate = 9600


constant_data = ''.join(chr(x) for x in [0x71, 0x72, 0x73, 0x74, 0x75, 0x76,0x77,0x78,0x79,0x60,0x61,0x62,0x63,0x64,0x65,0x66])

print "Using port: "+ port
conn = serial.Serial(port, baudrate)
if (not conn.isOpen()):
    conn.open()

buf = []

def print16bytes(data):
    for val in data:
        print format(ord(val),"x") + " ",
    print ""

def findMessageSeparator(buf):
    separatorFound = False
    while not separatorFound:
        if conn.inWaiting() > 0:
            print str(conn.inWaiting()) + " bytes available"
            bytesread = conn.read(conn.inWaiting())
            buf.extend(bytesread)
            try:
                zeroindex = buf.index(chr(0))
                print "found zero index: "+ str(zeroindex)
                print "Before: Length of buffer: "+ str(len(buf))
                buf = buf[zeroindex:]
                print "After:  Length of buffer: "+ str(len(buf))
                if len(buf) > 1:
                    print "index 1: "+ str(ord(buf[1]))
                    if ord(buf[1]) is 0:
                        separatorFound = True
                    else:
                        print "Discarding first two bytes: " + str(ord(buf[0])) +", " + str(ord(buf[1]))
                        buf.pop(2)
            except:
                print "No zero bytes, discarded buffer"
                return []
    print "Separator found"
    return buf

# Receive "start of transmission" bytes
# Loop and keep receiving TLV-frames ad infinitum.
# Read TL + correct number of bytes for V until done.

while True:
    buf = findMessageSeparator(buf)
    time.sleep(0.5)

