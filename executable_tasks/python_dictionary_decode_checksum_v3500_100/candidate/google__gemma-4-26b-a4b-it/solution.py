def run(ids, dict_vals, iters: int) -> int: 
    if iters <= 0:
        return 0
    
    # Since ids and dict_vals do not change, the 'decoded' array 
    # will be identical after every iteration.
    # The hash is reset to the initial constant at the start of every iteration.
    # Therefore, we only need to compute the hash once for the final state.
    
    # Pre-calculate the final state of decoded
    # We use a list comprehension for faster construction
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for v in decoded:
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h