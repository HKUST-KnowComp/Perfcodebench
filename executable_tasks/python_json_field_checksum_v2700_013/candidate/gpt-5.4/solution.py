import json


def run(rows: list[str]) -> int:
    loads = json.loads
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211

    for row in rows:
        obj = loads(row)
        h ^= ((obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)) + 1
        h = (h * mul) & mask

    return h
