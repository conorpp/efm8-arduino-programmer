import serial, sys, struct, time
from flask import Flask, request

app = Flask(__name__)


if len(sys.argv) < 2:
    print 'usage: %s <port> <serial-port1> [serial-port2...]' % sys.argv[0]
    sys.exit(1)

class PI():
    def __init__(self, com):
        self.ser = serial.Serial(com, 1000000, timeout = 1)

    def prog(self, firmware):

        print 'Connected'

        #f = open(firmware,'r').readlines()
        f = firmware.splitlines()

        # init Programming Interface (PI)
        self.ser.write('\x01\x00')
        x =struct.unpack('B', self.ser.read(1))[0]
        print 'x:',hex(x)
        assert(0x81 == x)

        print 'PI initiated'

        # erase device
        self.ser.write('\x04\x00')
        assert(0x84 == struct.unpack('B', self.ser.read(1))[0])

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
                self.ser.write([0x3, buf_size + 4, buf_size, 0, addrh, addrl])
                self.ser.write(buf.decode('hex'))
                assert(0x83 == struct.unpack('B', self.ser.read(1))[0])
                total += buf_size
                buf_size = 0
                buf = ''
                print 'Wrote %d bytes' % total

        # reset device
        self.ser.write('\x02\x00')
        assert(0x82 == struct.unpack('B', self.ser.read(1))[0])

        print 'Device reset'

print 'Once'
programmers = {}
for i in sys.argv[1:]:
    programmers[i] = PI(i)

@app.route('/', methods=['POST'])
def hello():
    port=request.form['port']
    firmware=request.form['firmware']

    programmers[port].prog(firmware)

    return 'Success'

app.debug = True
app.run(host='127.0.0.1', port = 4040, use_reloader=False, threaded = False)
