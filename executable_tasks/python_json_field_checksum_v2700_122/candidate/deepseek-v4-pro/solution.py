import json


def run(rows: list[str]) -> int:
    loads = json.loads
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF  # (1 << 64) - 1
    mul = 1099511628211
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h = (h ^ (v + 1)) * mul & mask
    return h
