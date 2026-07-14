import simdjson2

FNV_PRIME = 1099511628211
MASK_64 = (1 << 64) - 1
MASK_1023 = 1023

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    parser = simdjson2.Parser()
    for row in rows:
        obj = parser.parse(row)
        v = (obj["u"] & MASK_1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * FNV_PRIME) & MASK_64
    return h
