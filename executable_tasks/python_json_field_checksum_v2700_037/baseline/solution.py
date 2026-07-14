import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        obj = json.loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h
