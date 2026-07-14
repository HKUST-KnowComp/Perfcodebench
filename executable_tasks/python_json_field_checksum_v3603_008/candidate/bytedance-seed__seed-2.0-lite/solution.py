import orjson

FNV1_INIT = 1469598103934665603
FNV1_PRIME = 1099511628211
MASK64 = (1 << 64) - 1

def run(rows: list[str]) -> int:
    h = FNV1_INIT
    loads = orjson.loads
    mult = FNV1_PRIME
    mask = MASK64
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & 1023) + obj["s"] + (17 if obj["f"] else 0)
        h ^= v + 1
        h *= mult
        h &= mask
    return h