import orjson

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MUL = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    AND_MASK = 1023
    ADD_TRUE = 17
    for row in rows:
        obj = orjson.loads(row)
        v = (obj["u"] & AND_MASK) + obj["s"] + (ADD_TRUE if obj["f"] else 0)
        h ^= v + 1
        h = (h * MUL) & MASK
    return h
