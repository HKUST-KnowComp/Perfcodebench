import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MUL = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    loads = json.loads
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h = ((h ^ (v + 1)) * MUL) & MASK
    return h