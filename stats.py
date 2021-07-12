import requests
import math
import json

cookies = "session=8e493aa7-3270-4cbd-adf6-6027d3eeeb53; spockcookie=Oksn_N6VJHIC3QiHLMpPrO8q7oahK8n-WK_AuEr4MqWjM1Q1U07tezwqU5TvJ4zbJjIiY-E8-iwLgvIEJ5iBRA"
r = requests.get("https://poses.live/problems", headers={'cookie': cookies})
total = 0
total_best = 0
hc = {
    71: 8212,
    74: 4903,
    81: 0,
    109: 958,
    115: 46436,
    118: 4539,
    120: 17774,
    121: 14927,
}
for row in r.text.split("</tr><tr>"):
    if not row.startswith("<td>"):
        continue
    tok = row.replace("<", " ").replace(">", " ").split();
    test_id = tok[3]
    try:
        our = int(tok[7])
        best = int(tok[10])
    except:
        try:
            our = hc.get(int(test_id), -1)
            best = int(tok[9])
        except:
            our = hc.get(int(test_id), -1)
            best = int(tok[10])

    fin = "inputs/{0}.problem".format(test_id)
    with open(fin) as f:
        js = json.load(f)
        holes = len(js["hole"])
        vertices = len(js["figure"]["vertices"])
        edges = len(js["figure"]["edges"])
        coeff = math.log(holes * vertices * edges / 6.0) / math.log(2.0)

    score = math.ceil(1000 * coeff * math.sqrt((best + 1.0) / (our + 1.0))) if our >= 0 else 0
    best_score = math.ceil(1000 * coeff)
    loss = "- best!"
    if best_score > score:
        loss = "({0} lost)".format(best_score - score)
    print("{0:3s} | {1:5d} | {2:6d} = {3:6d} of {4} {5} {6}".format(test_id, our, best, score, best_score, "#" * ((best_score - score + 499) // 500), loss))
    total += score
    total_best += best_score

print("=" * 30)
print("Total: {0} of {1}".format(total, total_best))
