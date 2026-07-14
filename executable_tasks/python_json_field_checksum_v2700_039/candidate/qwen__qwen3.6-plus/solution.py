import json

def run(rows: list[str]) -> int:
    loads = json.loads
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for row in rows:
        d = loads(row)
        h ^= (d["u"] & 1023) + d["s"] + d["f"] * 17 + 1
        h = (h * prime) & mask
    return h