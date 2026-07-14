import orjson

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    loads = orjson.loads
    for row in rows:
        obj = loads(row)
        v = (obj[b"u"] & 1023) + obj[b"s"] + (17 if obj[b"f"] else 0)
        h ^= v + 1
        h *= 1099511628211
        h &= mask
    return h
