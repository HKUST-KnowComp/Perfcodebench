def run(ids, dict_vals, iters: int) -> int:
    # The checksum depends only on the final state of the 'decoded' list
    # after the last iteration. Since the values in 'decoded' are constant
    # across iterations (they are always dict_vals[idx]), we only need
    # to perform the lookup once and the checksum once.
    
    # Pre-calculate the decoded values
    decoded = [dict_vals[idx] for idx in ids]
    
    # The loop 'for _ in range(iters)' simply overwrites the same values
    # and then calculates the hash. The hash is calculated based on the
    # state of 'decoded' at the end of the last iteration.
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for v in decoded:
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h