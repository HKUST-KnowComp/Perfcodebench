try:
    import orjson as json
except ImportError:
    import json

INIT_HASH = 1469598103934665603
MUL = 1099511628211
MASK = (1 << 64) - 1

def run(rows: list[str]) -> int:
    loads = json.loads
    h = INIT_HASH
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * MUL) & MASK
    return h