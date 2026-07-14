def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-bind local variables for performance
    for _ in range(iters):
        hist = [0] * buckets
        # Using a local reference for the list to speed up access
        for k, v in zip(keys, vals):
            hist[k] += v
        
        # FNV-1a hash calculation
        h = 1469598103934665603
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h