import requests
import math
import json
import sys

test_id = None
if len(sys.argv) > 1:
  test_id = int(sys.argv[1])

convert = "text" in sys.argv

cookies = "session=8e493aa7-3270-4cbd-adf6-6027d3eeeb53; spockcookie=Oksn_N6VJHIC3QiHLMpPrO8q7oahK8n-WK_AuEr4MqWjM1Q1U07tezwqU5TvJ4zbJjIiY-E8-iwLgvIEJ5iBRA"
for i in range(1, 133):
  if test_id is not None and i != test_id:
      continue
  print('downloading ' + str(i))
  r = requests.get("https://poses.live/problems/" + str(i), headers={'cookie': cookies})
  w = b'a href="/solutions/'
  pos = r.content.find(w)
  sol = str(r.content[pos+len(w):pos+len(w)+36])[2:-1]
  r = requests.get("https://poses.live/solutions/" + sol + "/download", headers={'cookie': cookies})
  if convert:
    js = json.loads(r.content)
    v = js['vertices']
    with open(str(i) + ".ans", "w") as out:
      out.write("{0}\n".format(len(v)))
      for q in v:
        out.write(" ".join(map(str, q)) + "\n")
  else:
    open(str(i) + ".ans", 'wb').write(r.content)
