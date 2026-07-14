from json import loads

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK64 = 0xFFFFFFFFFFFFFFFF
    PRIME = 1099511628211
    
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h = (h * PRIME) & MASK64
    return h