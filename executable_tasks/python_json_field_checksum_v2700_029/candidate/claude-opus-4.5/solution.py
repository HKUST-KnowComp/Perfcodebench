import orjson

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    loads = orjson.loads
    for row in rows:
        obj = loads(row)
        v = (obj[b"u"] & 1023) + obj[b"s"] + (17 if obj[b"f"] else 0)
        h ^= v + 1
        h *= MULT
        h &= MASK
    return h
