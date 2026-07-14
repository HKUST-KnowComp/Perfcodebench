import json

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 18446744073709551615
    prime = 1099511628211
    loads = json.loads
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + 17 * obj["f"]
        h ^= v + 1
        h = (h * prime) & mask
    return h
