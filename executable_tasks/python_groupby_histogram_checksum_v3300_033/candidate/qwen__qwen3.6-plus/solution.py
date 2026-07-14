def run(keys, vals, buckets: int, iters: int) -> int:
    M = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    INIT = 1469598103934665603
    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = INIT
        for v in hist:
            h = ((h ^ (v + 1)) * M) & MASK
    return h