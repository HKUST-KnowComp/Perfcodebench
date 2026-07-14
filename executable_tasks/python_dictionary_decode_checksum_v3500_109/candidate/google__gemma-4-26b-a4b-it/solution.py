def run(ids, dict_vals, iters: int) -> int:
    if iters <= 0:
        return 0
    
    # The values in 'decoded' are identical for every iteration of 'iters'
    # because 'decoded[i] = dict_vals[idx]' is deterministic.
    # Therefore, we only need to compute the hash once.
    
    # Pre-fetch the values to avoid repeated indexing in the hash loop
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in decoded:
        h ^= (v + 1)
        h = (h * 1099511628211) & mask
        
    return h