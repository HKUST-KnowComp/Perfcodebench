import simdjson


def run(rows: list[str]) -> int:
    parser = simdjson.Parser()
    h = 1469598103934665603
    mask_64 = (1 << 64) - 1
    multiplier = 1099511628211
    
    for row in rows:
        obj = parser.parse(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * multiplier) & mask_64
    
    return h
