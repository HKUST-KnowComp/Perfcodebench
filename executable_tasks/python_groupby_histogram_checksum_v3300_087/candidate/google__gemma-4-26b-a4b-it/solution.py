def run(keys, vals, buckets: int, iters: int) -> int:
    # Pre-calculate constants
    MOD_MASK = (1 << 64) - 1
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211

    # Convert to local variables for faster access
    # We use a list for the histogram
    
    # To optimize the inner loop, we use a local reference to the list
    # and avoid zip() if possible, though zip() is quite fast in Python 3.
    
    # The baseline logic: 
    # for _ in range(iters):
    #     hist = [0] * buckets
    #     for k, v in zip(keys, vals):
    #         hist[k] += v
    #     h = FNV_OFFSET
    #     for v in hist:
    #         h ^= v + 1
    #         h = (h * FNV_PRIME) & MOD_MASK
    
    # Optimization: The loop structure is fixed. 
    # We can't use NumPy, so we focus on minimizing overhead.
    
    # Pre-zip to avoid overhead inside the iters loop
    zipped_data = list(zip(keys, vals))
    
    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zipped_data:
            hist[k] += v
        
        h = FNV_OFFSET
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MOD_MASK
            
    return h