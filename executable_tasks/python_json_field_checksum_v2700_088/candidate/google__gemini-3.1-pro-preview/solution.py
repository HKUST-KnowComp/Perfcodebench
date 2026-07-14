import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    loads = json.loads
    for row in rows:
        obj = loads(row)
        h = ((h ^ ((obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0) + 1)) * 1099511628211) & 18446744073709551615
    return h
