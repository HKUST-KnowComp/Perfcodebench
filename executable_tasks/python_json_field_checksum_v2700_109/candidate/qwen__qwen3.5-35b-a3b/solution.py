import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MULTIPLIER = 1099511628211
    for row in rows:
        obj = json.loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h ^= v + 1
        h *= MULTIPLIER
        h &= MASK
    return h