def run(keys, vals, buckets: int, iters: int) -> int: 
    # Pre-calculate constants for the FNV-1a style hash
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    # Convert to tuples once to speed up iteration if they aren't already
    # but zip is already quite fast.
    k_tuple = tuple(keys)
    v_tuple = tuple(vals)

    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        # Tight loop for aggregation
        for k, v in zip(k_tuple, v_tuple):
            hist[k] += v
        
        # Checksum calculation
        h = FNV_OFFSET
        for v in hist:
            h ^= (v + 1)
            h = (h * FNV_PRIME) & MASK
            
    return h