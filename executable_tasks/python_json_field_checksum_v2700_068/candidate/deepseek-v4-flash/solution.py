import json

def run(rows: list[str]) -> int:
    loads = json.loads
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    PRIME = 1099511628211
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h ^= v + 1
        h = (h * PRIME) & MASK
    return h