def run(ids, dict_vals, iters: int) -> int:
    h = 0
    mask = (1 << 64) - 1
    const_mult = 1099511628211
    init_hash = 1469598103934665603
    
    for _ in range(iters):
        h = init_hash
        for idx in ids:
            v = dict_vals[idx]
            h ^= v + 1
            h = (h * const_mult) & mask
    return h
