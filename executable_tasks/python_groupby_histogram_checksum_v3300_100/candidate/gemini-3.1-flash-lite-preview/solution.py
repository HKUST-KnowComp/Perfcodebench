def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-bind local variables for performance
    for _ in range(iters):
        hist = [0] * buckets
        # Using zip is standard, but direct indexing is faster if possible.
        # Given the constraints, we optimize the accumulation loop.
        for k, v in zip(keys, vals):
            hist[k] += v
        
        h = 1469598103934665603
        # Optimized FNV-1a hash loop
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h