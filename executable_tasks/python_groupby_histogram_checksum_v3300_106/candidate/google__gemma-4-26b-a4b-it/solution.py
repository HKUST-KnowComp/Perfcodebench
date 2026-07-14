def run(keys, vals, buckets: int, iters: int) -> int:
    # Pre-calculate constants to avoid repeated lookups
    MASK = (1 << 64) - 1
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211

    # Convert to tuples/lists once if they aren't already to speed up zip
    # but zip() is already an iterator.
    
    last_h = 0
    for _ in range(iters):
        hist = [0] * buckets
        # Tight loop for aggregation
        for k, v in zip(keys, vals):
            hist[k] += v
        
        # Optimized FNV-1a style checksum
        h = FNV_OFFSET
        for v in hist:
            h ^= (v + 1)
            h = (h * FNV_PRIME) & MASK
        last_h = h
        
    return last_h