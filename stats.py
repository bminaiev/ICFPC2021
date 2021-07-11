import requests
import math
import json

cookies = "session=8e493aa7-3270-4cbd-adf6-6027d3eeeb53; spockcookie=Oksn_N6VJHIC3QiHLMpPrO8q7oahK8n-WK_AuEr4MqWjM1Q1U07tezwqU5TvJ4zbJjIiY-E8-iwLgvIEJ5iBRA"
r = requests.get("https://poses.live/problems", headers={'cookie': cookies})
total = 0
total_best = 0
for row in r.text.split("</tr><tr>"):
    if not row.startswith("<td>"):
        continue
    tok = row.replace("<", " ").replace(">", " ").split();
    test_id = tok[3]
    try:
        our = int(tok[7])
        best = int(tok[10])
    except:
        our = 1000000
        best = 0

    fin = "inputs/{0}.problem".format(test_id)
    with open(fin) as f:
        js = json.load(f)
        holes = len(js["hole"])
        vertices = len(js["figure"]["vertices"])
        edges = len(js["figure"]["edges"])
        coeff = math.log(holes * vertices * edges / 6.0) / math.log(2.0)

    score = math.ceil(1000 * coeff * math.sqrt((best + 1.0) / (our + 1.0)))
    best_score = math.ceil(1000 * coeff)
    loss = "- best!"
    if best_score > score:
        loss = "({0} lost)".format(best_score - score)
    print("{0:3s} | {1:5d} | {2:5d} = {3:5d} of {4} {5}".format(test_id, our, best, score, best_score, loss))
    total += score
    total_best += best_score

print("=" * 30)
print("Total: {0} of {1}".format(total, total_best))