def run(keys, vals, buckets: int, iters: int) -> int: 
    # Pre-calculate constants for the FNV-1a hash
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    # Convert to local variables to speed up access
    # We use a single list and reuse it to avoid allocation overhead
    hist = [0] * buckets
    
    # Pre-zip to avoid repeated zip calls if possible, 
    # but since we must run 'iters' times, we do it inside.
    # To optimize, we use a local reference to the list's __setitem__ or similar if needed,
    # but standard indexing is usually fastest in CPython for this pattern.
    
    final_h = 0
    for _ in range(iters):
        # Reset histogram efficiently
        for i in range(buckets):
            hist[i] = 0
            
        # Aggregation loop
        for k, v in zip(keys, vals):
            hist[k] += v
            
        # Checksum calculation (FNV-1a style)
        h = FNV_OFFSET_BASIS
        for v in hist:
            h ^= (v + 1)
            h = (h * FNV_PRIME) & MASK
        final_h = h
        
    return final_h