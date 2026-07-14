import json


def run(rows: list[str]) -> int:
    MASK = (1 << 64) - 1
    MULTIPLIER = 1099511628211
    INITIAL_H = 1469598103934665603
    h = INITIAL_H
    for row in rows:
        obj = json.loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h ^= v + 1
        h *= MULTIPLIER
        h &= MASK
    return h