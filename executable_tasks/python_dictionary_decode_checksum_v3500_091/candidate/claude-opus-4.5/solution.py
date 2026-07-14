def run(ids, dict_vals, iters: int) -> int:
    # Decode once - result is the same every iteration
    decoded = [dict_vals[idx] for idx in ids]
    
    # Cache constants and mask as locals for faster access
    INIT = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        h = INIT
        for v in decoded:
            h = ((h ^ (v + 1)) * MULT) & MASK
    return h
