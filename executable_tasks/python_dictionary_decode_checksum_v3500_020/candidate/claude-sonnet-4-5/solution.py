def run(ids, dict_vals, iters: int) -> int:
    h = 0
    HASH_INIT = 1469598103934665603
    HASH_MULT = 1099511628211
    MASK = (1 << 64) - 1
    
    for _ in range(iters):
        h = HASH_INIT
        for idx in ids:
            v = dict_vals[idx]
            h ^= v + 1
            h = (h * HASH_MULT) & MASK
    return h
