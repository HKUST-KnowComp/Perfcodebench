import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211
    loads = json.loads
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + obj["f"] * 17
        h ^= v + 1
        h = (h * mul) & mask
    return h
