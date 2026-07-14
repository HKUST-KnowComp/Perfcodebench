def run(ids, dict_vals, iters: int) -> int:
    h = 1469598103934665603
    # Pre-calculate the constant to avoid repeated bitwise operations
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    # Cache local references to speed up loop lookups
    vals = dict_vals
    
    for _ in range(iters):
        h = 1469598103934665603
        for idx in ids:
            # Inline the FNV-1a hash logic to avoid list overhead
            h = ((h ^ (vals[idx] + 1)) * FNV_PRIME) & MASK
    return h