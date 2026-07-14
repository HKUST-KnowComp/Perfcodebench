import json

def run(rows: list[str]) -> int:
    objs = json.loads('[' + ','.join(rows) + ']')
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for obj in objs:
        v = (obj['u'] & 1023) + obj['s'] + (17 if obj['f'] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h