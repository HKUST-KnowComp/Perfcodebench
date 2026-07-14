import json


def run(rows: list[str]) -> int:
    loads = json.loads
    MASK = (1 << 64) - 1
    h = 1469598103934665603
    for row in rows:
        obj = loads(row)
        u = obj["u"] & 1023
        s = obj["s"]
        f = 17 if obj["f"] else 0
        v = u + s + f
        h ^= v + 1
        h = (h * 1099511628211) & MASK
    return h
