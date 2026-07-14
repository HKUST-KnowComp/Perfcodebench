import orjson


def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    
    for row in rows:
        obj = orjson.loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * mult) & mask
    
    return h
