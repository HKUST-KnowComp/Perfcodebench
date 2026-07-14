def run(keys, vals, buckets: int, iters: int) -> int:
    MASK = (1 << 64) - 1
    MULTIPLIER = 1099511628211
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * MULTIPLIER) & MASK
    return h