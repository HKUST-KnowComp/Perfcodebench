def run(ids, dict_vals, iters: int) -> int: 
    if not ids:
        return 1469598103934665603
    
    # The values in 'decoded' are identical in every iteration of 'iters'
    # because 'decoded[i] = dict_vals[idx]' always results in the same array.
    # Therefore, we only need to perform the assignment once and the hash once.
    
    # Pre-calculate the decoded array for the final state
    # Using a list comprehension is faster than a manual loop for assignment
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    for v in decoded:
        h ^= (v + 1)
        h = (h * 1099511628211) & mask
        
    return h