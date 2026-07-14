def run(ids, dict_vals, iters: int) -> int:
    if iters <= 0:
        return 0
    
    # Decode values using list comprehension for performance
    decoded = [dict_vals[idx] for idx in ids]
    
    # FNV-1a 64-bit hash constants
    h = 1469598103934665603
    PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    
    for v in decoded:
        h ^= v + 1
        h = (h * PRIME) & MASK
    
    return h