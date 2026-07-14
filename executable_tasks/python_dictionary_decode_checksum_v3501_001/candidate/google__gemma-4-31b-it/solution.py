def run(ids, dict_vals, iters: int) -> int:
    # The decoded values are the same for every iteration
    decoded = [dict_vals[idx] for idx in ids]
    
    # Pre-calculate constants for the checksum loop
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    # The checksum depends only on the final state of the decoded list
    # and the number of iterations. However, the logic provided in the baseline
    # resets 'h' inside the iters loop, meaning only the last iteration matters.
    # If the baseline intended to accumulate 'h' across iterations, it would not
    # reset h = 1469598103934665603. 
    # Since it resets, we only need to perform the calculation once.
    
    h = 1469598103934665603
    for v in decoded:
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h