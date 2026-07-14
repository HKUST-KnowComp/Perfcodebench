import orjson

def run(rows: list[str]) -> int:
    # Bind all constants and parser to local scope for fast access
    loads = orjson.loads
    mask_1023 = 1023
    f_bonus = 17
    fnv_prime = 1099511628211
    u64_mask = (1 << 64) - 1
    h = 1469598103934665603
    
    for row in rows:
        obj = loads(row)
        v = (obj["u"] & mask_1023) + obj["s"] + (f_bonus if obj["f"] else 0)
        h ^= v + 1
        h *= fnv_prime
        h &= u64_mask
    return h