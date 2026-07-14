def run(ids, dict_vals, iters: int) -> int:
    h = 0
    PRIME = 1099511628211
    MASK = (1 << 64) - 1
    INIT = 1469598103934665603
    
    for _ in range(iters):
        h = INIT
        for idx in ids:
            v = dict_vals[idx]
            h ^= v + 1
            h = (h * PRIME) & MASK
    return h
