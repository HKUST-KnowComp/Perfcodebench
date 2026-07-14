def run(ids, dict_vals, iters: int) -> int:
    h = 0
    mask = (1 << 64) - 1
    const = 1099511628211
    
    for _ in range(iters):
        h = 1469598103934665603
        for idx in ids:
            v = dict_vals[idx]
            h ^= v + 1
            h = (h * const) & mask
    
    return h
