def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch values to avoid repeated dictionary lookups if dict_vals is a dict
    # If dict_vals is a list, this is already fast. 
    # We map ids to values once to avoid repeated lookups inside the loop.
    vals = [dict_vals[i] for i in ids]
    
    h = 0
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    for _ in range(iters):
        h = FNV_OFFSET
        for v in vals:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
            
    return h