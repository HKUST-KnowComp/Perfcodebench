def run(ids, dict_vals, iters: int) -> int:
    # Pre-decode the values since ids and dict_vals do not change across iterations
    decoded = [dict_vals[idx] for idx in ids]
    
    # Localize constants for faster access
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    initial_h = 1469598103934665603
    
    h = 0
    for _ in range(iters):
        h = initial_h
        for v in decoded:
            h ^= v + 1
            h = (h * multiplier) & mask
            
    return h