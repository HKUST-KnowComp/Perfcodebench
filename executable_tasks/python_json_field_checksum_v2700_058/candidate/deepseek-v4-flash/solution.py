import json

def run(rows: list[str]) -> int:
    _loads = json.loads
    h = 1469598103934665603
    mult = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    for row in rows:
        obj = _loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * mult) & mask
    return h