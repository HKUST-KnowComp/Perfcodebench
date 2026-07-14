from json import loads

def run(rows: list[str]) -> int:
    MASK = 1023
    MULT = 1099511628211
    MOD = (1 << 64) - 1
    h = 1469598103934665603
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & MASK) + obj["s"] + obj["f"] * 17
        h = ((h ^ (v + 1)) * MULT) & MOD
    return h
