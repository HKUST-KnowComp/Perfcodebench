def run(keys, vals, buckets: int, iters: int) -> int:
    MASK = 0xFFFFFFFFFFFFFFFF
    FNV_PRIME = 1099511628211
    FNV_INIT = 1469598103934665603
    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = FNV_INIT
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
    return h