import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    _loads = json.loads
    MASK = (1 << 64) - 1
    MULTIPLIER = 1099511628211
    MASK_U = 1023
    OFFSET = 17
    for row in rows:
        obj = _loads(row)
        u = obj["u"]
        s = obj["s"]
        f = obj["f"]
        v = (u & MASK_U) + s + (f * OFFSET)
        h ^= v + 1
        h *= MULTIPLIER
        h &= MASK
    return h