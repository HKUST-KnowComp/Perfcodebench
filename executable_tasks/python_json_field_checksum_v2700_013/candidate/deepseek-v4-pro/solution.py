import json

def run(rows: list[str]) -> int:
    MASK = 0xFFFFFFFFFFFFFFFF
    MULT = 1099511628211
    h = 1469598103934665603
    loads = json.loads
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + obj["f"] * 17
        h = (h ^ (v + 1)) * MULT & MASK
    return h
