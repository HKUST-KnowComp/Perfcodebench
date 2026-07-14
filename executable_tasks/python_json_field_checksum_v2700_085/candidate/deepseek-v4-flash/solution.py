import json

def run(rows: list[str]) -> int:
    combined = '[' + ','.join(rows) + ']'
    objs = json.loads(combined)
    h = 1469598103934665603
    for obj in objs:
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h