import serial, sys, struct, time

if len(sys.argv) != 3:
    print 'usage: %s <serial-port> <firmware.hex>' % sys.argv[0]
    sys.exit(1)

ser = serial.Serial(sys.argv[1], 1000000, timeout = 1)
time.sleep(1)


print 'Connected'

f = open(sys.argv[2],'r').readlines()

# init Programming Interface (PI)
ser.write('\x01\x00')
x =struct.unpack('B', ser.read(1))[0]
print 'x:',hex(x)
assert(0x81 == x)
print 'read 2 bytes'

print 'PI initiated'

# erase device
ser.write('\x04\x00')
assert(0x84 == struct.unpack('B', ser.read(1))[0])

print 'Device erased'

# write hex file
total = 0
buf = ''
buf_size = 0
for i in f[1:-1]:  # skip first and second lines
    assert(i[0] == ':')
    size = int(i[1:3],16)
    assert(size + 4 < 256)
    if buf_size == 0:
        addrh = int(i[3:5],16)
        addrl = int(i[5:7],16)
    assert(i[7:9] == '00')
    data = i[9:9 + size*2]
    assert(len(data) == size*2)

    buf += data
    buf_size += size

    if buf_size > 175:
        #print hex(addrh), hex(addrl), buf
        ser.write([0x3, buf_size + 4, buf_size, 0, addrh, addrl])
        ser.write(buf.decode('hex'))
        assert(0x83 == struct.unpack('B', ser.read(1))[0])
        total += buf_size
        buf_size = 0
        buf = ''
        print 'Wrote %d bytes' % total

# reset device
ser.write('\x02\x00')
assert(0x82 == struct.unpack('B', ser.read(1))[0])

print 'Device reset'


