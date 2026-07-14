def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals as a local variable for faster access
    # The checksum logic is FNV-1a variant
    h_init = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1
    
    # Pre-map ids to values to avoid repeated lookups in the inner loop
    decoded = [dict_vals[i] for i in ids]
    
    h = h_init
    for _ in range(iters):
        h = h_init
        for v in decoded:
            h = ((h ^ (v + 1)) * fnv_prime) & mask
    return h