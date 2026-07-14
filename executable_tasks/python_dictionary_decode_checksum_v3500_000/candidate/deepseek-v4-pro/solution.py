def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    # precompute (value + 1) for each key to avoid dict lookups in inner loop
    vals = [dict_vals[key] + 1 for key in ids]
    SEED = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = SEED
        for v in vals:
            h ^= v
            h = (h * MULT) & MASK
    return h
