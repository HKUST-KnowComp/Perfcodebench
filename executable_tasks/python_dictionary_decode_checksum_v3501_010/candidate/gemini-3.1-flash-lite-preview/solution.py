def run(ids, dict_vals, iters: int) -> int:
    h = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    # Pre-fetch dict_vals to local variable for faster access
    vals = dict_vals
    
    for _ in range(iters):
        h = 1469598103934665603
        for idx in ids:
            h ^= (vals[idx] + 1)
            h = (h * FNV_PRIME) & MASK
    return h