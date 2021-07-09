import requests as r
import json
import sys
import os

apikey = 'fbb4e890-43a5-4864-92be-d31513e47acd'

if len(sys.argv) != 2:
  print('usage: python sub.py [test-id]')
  exit()

test = sys.argv[1]
try:
  gena_wants_to_be_sure_test_id_is_int = int(test)
except:
  print('test-id must be an integer')
  exit()

cmd = ('call "../gennady/score.exe" ' if os.name == 'nt' else '../gennady/score ') + test
try:
  code = os.system(cmd)
except:
  print('can not check file')
  exit()

if code != 0:
  exit()

points = []
with open(test + '.ans') as f:
  expected = -1
  for line in f:
    line = list(map(int, line.strip().split()))

    if len(line) == 1:
      expected = line[0]

    if len(line) == 2:
      points.append(line)

if expected < 2:
  print('expected less than 2 points (maybe N is missing in the first line?)')
  exit()

if len(points) != expected:
  print('wrong number of points: expected {0}, found {1}'.format(expected, len(points)))
  exit()

ans = json.dumps({'vertices': points})
print('submitting ' + ans)

s = r.Session()
r = s.post('https://poses.live/api/problems/' + str(test) + '/solutions', headers={'Authorization': 'Bearer ' + apikey}, data=ans)
print(r.content)
