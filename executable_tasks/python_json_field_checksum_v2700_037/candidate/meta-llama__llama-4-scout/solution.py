import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    decoder = json.JSONDecoder()
    for row in rows:
        obj = decoder.raw_decode(row)[0]
        v = (obj[u"u"] & 1023) + obj[u"s"] + (17 if obj[u"f"] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h