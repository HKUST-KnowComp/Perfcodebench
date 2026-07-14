import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    if not rows:
        return h
    # Parse all rows as a single JSON array
    objs = json.loads('[' + ','.join(rows) + ']')
    for obj in objs:
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h = (h * mult) & mask
    return h