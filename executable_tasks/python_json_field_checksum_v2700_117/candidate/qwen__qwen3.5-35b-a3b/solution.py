import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = 0xFFFFFFFFFFFFFFFF
    MULTIPLIER = 1099511628211
    for row in rows:
        obj = json.loads(row)
        u = obj["u"]
        s = obj["s"]
        f = obj["f"]
        v = (u & 1023) + s + 17 * f
        h = ((h ^ (v + 1)) * MULTIPLIER) & MASK
    return h