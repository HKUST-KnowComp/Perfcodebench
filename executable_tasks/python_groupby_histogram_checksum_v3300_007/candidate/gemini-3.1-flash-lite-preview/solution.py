def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-bind local variables for speed
    for _ in range(iters):
        hist = [0] * buckets
        # Using a local loop over zip is standard, but we can optimize the accumulation
        for k, v in zip(keys, vals):
            hist[k] += v
        
        h = 1469598103934665603
        # FNV-1a hash calculation
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h