def run(keys, vals, buckets, iters):
    if iters <= 0:
        return 0
    hist = [0] * buckets
    n = len(keys)
    for i in range(n):
        hist[keys[i]] += vals[i]
    h = 1469598103934665603
    M = 1099511628211
    MASK = (1 << 64) - 1
    for v in hist:
        h ^= v + 1
        h = (h * M) & MASK
    return h