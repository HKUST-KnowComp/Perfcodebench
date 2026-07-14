import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    for row in rows:
        obj = json.loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h = (h ^ (v + 1)) * FNV_PRIME & MASK
    return h