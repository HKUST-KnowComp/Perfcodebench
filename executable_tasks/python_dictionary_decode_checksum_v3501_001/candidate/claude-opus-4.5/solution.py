def run(ids, dict_vals, iters: int) -> int:
    # Decode once since ids and dict_vals are constant
    decoded = [dict_vals[idx] for idx in ids]
    
    # Precompute constants
    INIT = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    
    # Run hash computation iters times
    h = 0
    for _ in range(iters):
        h = INIT
        for v in decoded:
            h = ((h ^ (v + 1)) * MULT) & MASK
    return h
