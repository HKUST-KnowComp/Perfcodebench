def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals to local variable for faster access
    # Pre-calculate the decoded values once since they are constant across iterations
    decoded = [dict_vals[i] for i in ids]
    
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        h = 1469598103934665603
        for v in decoded:
            h = ((h ^ (v + 1)) * fnv_prime) & mask
            
    return h