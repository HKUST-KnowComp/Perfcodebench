def run(keys, vals, buckets: int, iters: int) -> int:
    hist = [0] * buckets
    h = 0
    for _ in range(iters):
        for i in range(buckets):
            hist[i] = 0
        for k, v in zip(keys, vals):
            hist[k] += v
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
