def run(ids, dict_vals, iters: int) -> int:
    # The values in 'decoded' are the same for every iteration of the outer loop.
    # The checksum 'h' is reset at the start of every iteration.
    # Therefore, the result is identical to running the process once.
    
    # Pre-calculate the decoded values once
    decoded = [dict_vals[idx] for idx in ids]
    
    # Calculate the checksum once
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in decoded:
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h