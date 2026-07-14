def run(ids, dict_vals, iters: int) -> int:
    # Pre-calculate constants to avoid repeated lookups
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    # Localize variables for faster access
    local_ids = ids
    local_dict_vals = dict_vals
    
    h = 0
    for _ in range(iters):
        h = FNV_OFFSET_BASIS
        for idx in local_ids:
            # Combine lookup and hash update to avoid intermediate list
            v = local_dict_vals[idx]
            h ^= v + 1
            h = (h * FNV_PRIME) & MASK
            
    return h