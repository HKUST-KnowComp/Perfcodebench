def run(ids, dict_vals, iters: int) -> int:
    h_init = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    
    # Pre-fetch dict_vals access to local variable for speed
    vals = dict_vals
    
    # We need to track the 'decoded' state across iterations.
    # However, the current logic re-assigns decoded[i] = dict_vals[idx] every iteration.
    # This means 'decoded' is always the same sequence of values after the first iteration.
    
    # Step 1: Compute the sequence once
    sequence = [vals[idx] for idx in ids]
    
    # Step 2: Perform the iterations
    # The first iteration populates 'decoded' (which is 'sequence')
    # Subsequent iterations do the same thing to 'decoded', so 'decoded' remains 'sequence'.
    # The hash is recalculated from scratch every iteration using the current 'decoded'.
    
    for _ in range(iters):
        h = h_init
        for v in sequence:
            h ^= v + 1
            h = (h * multiplier) & mask
            
    return h