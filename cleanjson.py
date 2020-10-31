import json

fin = open("AtomicCards.json", "r")

data = json.load(fin)

fin.close()

fout = open("AtomicCardsNeededOnly.json", "w")

data = data["data"]
cleaned = {}
for card in data:
    manacost = None
    if "manaCost" in data[card][0]:
        manacost = data[card][0]["manaCost"]
    cleaned[card] = {"colorIdentity": data[card][0]["colorIdentity"], "colors":data[card][0]["colors"], "convertedManaCost":data[card][0]["convertedManaCost"], "manaCost":manacost, "name":data[card][0]["name"], "type":data[card][0]["type"]}

fout.write(json.dumps(cleaned))