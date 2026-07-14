def run(keys, vals, buckets: int, iters: int) -> int:
    MASK = 0xFFFFFFFFFFFFFFFF
    PRIME = 1099511628211
    OFFSET = 1469598103934665603
    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = OFFSET
        for v in hist:
            h = (h ^ v + 1) * PRIME & MASK
    return h