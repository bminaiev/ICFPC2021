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
  f = open(str(test) + '.ans', 'r')
except:
  print('file ' + str(test) + '.ans not found')
  exit()

ans = '{"vertices":['

expected = -1
cnt = 0
for line in f.readlines():
  line = list(map(int, line.strip().split()))

  if len(line) == 1:
    expected = line[0]
  
  if len(line) == 2:
    if cnt > 0:
      ans += ','
    cnt += 1
    ans += '[' + str(line[0]) + ',' + str(line[1]) + ']'

f.close()

if expected < 2:
  print('expected less than 2 points (maybe N is missing in the first line?)')
  exit()
  
if cnt != expected:
  print('wrong number of points: expected ' + str(expected) + ', found ' + str(cnt))
  exit()

ans += ']}'
print('submitting ' + ans)

s = r.Session()
r = s.post('https://poses.live/api/problems/' + str(test) + '/solutions', headers={'Authorization': 'Bearer ' + apikey}, data=ans)
print(r.content)
