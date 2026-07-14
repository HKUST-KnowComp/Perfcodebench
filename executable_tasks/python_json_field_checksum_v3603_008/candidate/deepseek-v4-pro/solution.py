import orjson

MASK64 = (1 << 64) - 1
MULT = 1099511628211
INIT = 1469598103934665603

def run(rows: list[str]) -> int:
    h = INIT
    mask = MASK64
    mult = MULT
    loads = orjson.loads
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h = ((h ^ (v + 1)) * mult) & mask
    return h
