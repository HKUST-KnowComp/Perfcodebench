def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals to local variable for faster access
    # Pre-calculate the mapping to avoid repeated lookups if possible,
    # but here we directly map ids to values.
    vals = [dict_vals[i] for i in ids]
    
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        h = 1469598103934665603
        for v in vals:
            h = ((h ^ (v + 1)) * fnv_prime) & mask
            
    return h