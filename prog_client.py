import requests, sys


if len(sys.argv) != 3:
    print 'usage: %s <port> <firmware.hex>' % sys.argv[0]
    sys.exit(1)

url = 'http://127.0.0.1:4040/'

payload = {'port': sys.argv[1],  'firmware': open(sys.argv[2], 'r').read()}

print requests.post(url, data = payload)

