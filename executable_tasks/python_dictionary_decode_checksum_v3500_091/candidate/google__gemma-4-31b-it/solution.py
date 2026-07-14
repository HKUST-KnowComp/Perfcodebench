def run(ids, dict_vals, iters: int) -> int:
    # Pre-calculate constants
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    START_H = 1469598103934665603
    
    # Localize variables for faster access
    local_ids = ids
    local_vals = dict_vals
    
    h = 0
    for _ in range(iters):
        h = START_H
        for idx in local_ids:
            v = local_vals[idx]
            h ^= v + 1
            h = (h * MULT) & MASK
    return h