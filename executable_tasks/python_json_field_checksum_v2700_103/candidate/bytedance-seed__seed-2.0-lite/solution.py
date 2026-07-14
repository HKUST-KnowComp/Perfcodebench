import orjson

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    load = orjson.loads
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    for row in rows:
        obj = load(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= fnv_prime
        h &= mask
    return h