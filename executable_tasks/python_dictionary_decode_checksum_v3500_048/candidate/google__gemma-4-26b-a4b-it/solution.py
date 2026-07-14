def run(ids, dict_vals, iters: int) -> int: 
    # Pre-fetch the values to avoid repeated dictionary lookups in the iters loop
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    # The inner loop performs the same operation 'iters' times.
    # However, the checksum 'h' depends on the state of the previous iteration.
    # We must execute the loop 'iters' times.
    
    for _ in range(iters):
        # The 'decoded' list is actually constant across 'iters' because 
        # dict_vals[idx] doesn't change. We can optimize the hashing.
        for v in decoded:
            h ^= v + 1
            h = (h * 1099511628211) & mask
            
    return h