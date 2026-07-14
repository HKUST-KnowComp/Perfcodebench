import json


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for row in rows:
        obj = json.loads(row)
        u = obj["u"]
        s = obj["s"]
        f = obj["f"]
        v = (u & 1023) + s + 17 * f
        h ^= v + 1
        h *= prime
        h &= mask
    return h