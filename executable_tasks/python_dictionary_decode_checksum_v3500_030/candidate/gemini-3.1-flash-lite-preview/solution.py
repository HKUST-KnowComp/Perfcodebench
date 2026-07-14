def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dictionary values into a local list for faster access
    # and avoid repeated indexing overhead.
    decoded = [dict_vals[i] for i in ids]
    
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        curr_h = 1469598103934665603
        for v in decoded:
            curr_h = ((curr_h ^ (v + 1)) * fnv_prime) & mask
        h = curr_h
        
    return h