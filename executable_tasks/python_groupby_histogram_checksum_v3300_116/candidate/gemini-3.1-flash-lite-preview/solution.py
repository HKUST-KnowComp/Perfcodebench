def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-bind local variables for faster access in the loop
    for _ in range(iters):
        hist = [0] * buckets
        # Using a single loop with zip is standard, but we can optimize the inner loop
        for k, v in zip(keys, vals):
            hist[k] += v
        
        # FNV-1a hash calculation
        h = 1469598103934665603
        # Localize constants and operations
        fnv_prime = 1099511628211
        mask = (1 << 64) - 1
        for v in hist:
            h = ((h ^ (v + 1)) * fnv_prime) & mask
    return h