def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals for faster access
    # Using a local variable for the hash constant and mask
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    # Pre-map ids to values to avoid repeated dictionary lookups if dict_vals is a dict
    # If dict_vals is a list, direct indexing is already fast.
    vals = [dict_vals[i] for i in ids]
    
    h = FNV_OFFSET
    for _ in range(iters):
        h = FNV_OFFSET
        for v in vals:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
    return h