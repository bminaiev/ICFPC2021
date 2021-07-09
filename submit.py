import requests as r
import sys

apikey = 'fbb4e890-43a5-4864-92be-d31513e47acd'

if len(sys.argv) != 2:
  print('usage: python submit.py [test-id]')
  exit()

test = sys.argv[1]
try:
  test = int(test)
except:
  print('test-id must be an integer')
  exit()

try:
  output = open(str(test) + '.ans', 'r').read()
except:
  print('file ' + str(test) + '.ans not found')
  exit()

s = r.Session()
r = s.post('https://poses.live/api/problems/' + str(test) + '/solutions', headers={'Authorization': 'Bearer ' + apikey}, data=output)
print(r.content)
