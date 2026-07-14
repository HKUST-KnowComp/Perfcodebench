import json

def run(rows: list[str]) -> int:
    if not rows:
        return 1469598103934665603
    combined = '[' + ','.join(rows) + ']'
    data = json.loads(combined)
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for obj in data:
        v = (obj["u"] & 1023) + obj["s"] + obj["f"] * 17
        h ^= v + 1
        h = (h * mult) & mask
    return h
