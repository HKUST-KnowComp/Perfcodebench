import simdjson

parser = simdjson.Parser()
MASK = (1 << 64) - 1
MULT = 1099511628211

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        obj = parser.parse(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * MULT) & MASK
    return h
