def run(ids, dict_vals, iters: int) -> int:
    # Pre-compute constants and cache lookups
    MASK = 0xFFFFFFFFFFFFFFFF
    MULT = 1099511628211
    INIT = 1469598103934665603
    
    # Pre-decode once - the decoded values are the same every iteration
    # since ids and dict_vals don't change
    decoded = tuple(dict_vals[idx] for idx in ids)
    
    # Pre-compute (v + 1) values since they're constant
    vals_plus_one = tuple(v + 1 for v in decoded)
    
    h = 0
    for _ in range(iters):
        h = INIT
        for vp1 in vals_plus_one:
            h = ((h ^ vp1) * MULT) & MASK
    
    return h
