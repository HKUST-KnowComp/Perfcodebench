def run(keys, vals, buckets: int, iters: int) -> int:
    hist = [0] * buckets
    h_seed = 1469598103934665603
    mult = 1099511628211 & ((1 << 64) - 1)
    h = 0
    for _ in range(iters):
        for k, v in zip(keys, vals):
            hist[k] += v
        h = h_seed
        for v in hist:
            h ^= v + 1
            h = (h * mult) & ((1 << 64) - 1)
        hist = [0] * buckets
    return h