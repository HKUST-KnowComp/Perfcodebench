import orjson

def run(rows: list[str]) -> int:
    load_json = orjson.loads
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    h = 1469598103934665603
    for row in rows:
        obj = load_json(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= fnv_prime
        h &= mask
    return h