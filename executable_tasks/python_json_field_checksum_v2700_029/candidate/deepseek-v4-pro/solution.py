import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mult = 1099511628211
    loads = json.loads
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 0x3FF) + obj["s"] + 17 * obj["f"] + 1
        h = (h ^ v) * mult & mask
    return h