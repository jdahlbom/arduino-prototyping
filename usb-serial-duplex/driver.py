import serial
import sys
import time

port = sys.argv[1]
baudrate = 9600

TYPE_CONTROL = 1
TYPE_STRING = 2

TX_POWER_MAX = 122
TX_POWER_MIN = 65
GAME_POWER_MAX = 300
GAME_POWER_MIN = 0

TX_COOLANT_DELTA_OFFSET = 67
TX_COOLANT_DELTA_MAX = 122
TX_COOLANE_DELTA_MIN = 65

power = [100, 100, 100, 100, 100, 100, 100, 100]
coolant = [0, 0, 0, 0, 0, 0, 0, 0]

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

def readSerialIn(buf):
    if conn.inWaiting() > 0:
        print str(conn.inWaiting()) + " bytes available"
    bytesread = conn.read(conn.inWaiting())
    buf.extend(bytesread)
    print "Buffer length: "+ str(len(buf))
    return buf

def findMessageSeparator(buf):
    separatorFound = False
    while len(buf) > 1 and not separatorFound:
        try:
            zeroindex = buf.index(chr(0))
            buf = buf[zeroindex:]
            if len(buf) > 1:
                if ord(buf[1]) is 0:
                    separatorFound = True
                else:
                    print "Discarding first two bytes: " + str(ord(buf[0])) +", " + str(ord(buf[1]))
                    if len(buf) > 2:
                        buf = buf[2:]
                    else:
                        buf = []
        except:
            print "No zero bytes, discarded buffer"
            buf = []
    return buf

def descaleCoolant(txCoolant):
    return ord(txCoolant) - TX_COOLANT_DELTA_OFFSET

def descalePower(txPower):
    return ord(txPower)-TX_POWER_MIN/(TX_POWER_MAX-TX_POWER_MIN)*(GAME_POWER_MAX-GAME_POWER_MIN)+GAME_POWER_MIN

def handleControl(data):
    if len(data) is not 16:
        print "Wrong length for control data!"
    else:
        print "Control data is right length."
        txPowers = data[:8]
        txCoolants = data[8:]
        global power
        global coolant
        power = map(descalePower, txPowers)
        coolant = map(descaleCoolant, txCoolants)
        print "=== CONTROL MESSAGE RESULT ==="
        print "POWER: " + ",".join(map(str, power))
        print "COOLANT: " + ",".join(map(str, coolant))
        print "=== CONTROL MESSAGE ENDED  ==="

def handleString(data):
    print "Handling string with data length "+ str(len(data))
    print "====== Start message:  ======="
    print "".join(data)
    print "====== End of message: ======="

def processMessage(buf):
    while len(buf) < 4:
        bytesread = conn.read(conn.inWaiting())
        buf.extend(bytesread)
    type = ord(buf[2])
    length = ord(buf[3])
    buf = buf[4:]
    print "Type: "+ str(type) + ", Length: "+ str(length)
    print "Length of buffer: "+ str(len(buf))
    while len(buf) < length:
        bytesread = conn.read(conn.inWaiting())
        buf.extend(bytesread)
    msgdata = buf[:length]
    if type is TYPE_CONTROL:
        handleControl(msgdata)
    elif type is TYPE_STRING:
        handleString(msgdata)
    return buf[length:]


# Receive "start of transmission" bytes
# Loop and keep receiving TLV-frames ad infinitum.
# Read TL + correct number of bytes for V until done.

while True:
    buf = readSerialIn(buf)
    while len(buf) > 1:
        buf = findMessageSeparator(buf)
        buf = processMessage(buf)
        buf = readSerialIn(buf)
        time.sleep(0.01)
    time.sleep(0.05)

