def run(ids, dict_vals, iters: int) -> int:
    # Pre-compute constants and use local references for speed
    MASK = (1 << 64) - 1
    PRIME = 1099511628211
    
    # Local references to avoid repeated lookups
    local_ids = ids
    local_dict = dict_vals
    
    h = 0
    for _ in range(iters):
        # Decode using list comprehension (faster than enumerate loop)
        decoded = [local_dict[idx] for idx in local_ids]
        
        # Compute hash with local constants
        h = 1469598103934665603
        for v in decoded:
            h ^= v + 1
            h = (h * PRIME) & MASK
    
    return h
