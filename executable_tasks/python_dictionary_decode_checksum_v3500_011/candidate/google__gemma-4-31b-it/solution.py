def run(ids, dict_vals, iters: int) -> int:
    # The values in 'decoded' are the same for every iteration of the outer loop.
    # Therefore, the final hash 'h' will be the same as the hash computed in a single iteration.
    # We only need to perform the computation once.
    
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for v in decoded:
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h