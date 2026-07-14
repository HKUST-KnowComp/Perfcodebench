def run(ids, dict_vals, iters: int) -> int:
    # Cache constants and functions to local variables for faster access
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    # Pre-fetch the values to avoid repeated dictionary lookups if possible,
    # but the task requires the loop to run 'iters' times. 
    # If the values don't change, we can pre-calculate the sequence.
    vals = [dict_vals[idx] for idx in ids]
    
    h = 0
    for _ in range(iters):
        h = FNV_OFFSET_BASIS
        for v in vals:
            h ^= v + 1
            h = (h * FNV_PRIME) & MASK
            
    return h