def run(ids, dict_vals, iters: int) -> int:
    # Pre-calculate the values to avoid repeated indexing in the inner loop
    # if iters is large, caching the decoded values once is faster.
    decoded_vals = [dict_vals[idx] for idx in ids]
    
    # Local variables for faster access
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for _ in range(iters):
        h = 1469598103934665603
        for v in decoded_vals:
            h = ((h ^ (v + 1)) * multiplier) & mask
            
    return h