def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-calculate constants for FNV-1a
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for _ in range(iters):
        hist = [0] * buckets
        # Local variable access is faster in Python loops
        for k, v in zip(keys, vals):
            hist[k] += v
        
        h = FNV_OFFSET
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
    return h