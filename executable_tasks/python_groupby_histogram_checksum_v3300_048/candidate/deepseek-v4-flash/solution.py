def run(keys, vals, buckets: int, iters: int) -> int:
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    for v in hist:
        h ^= v + 1
        h = (h * prime) & mask
    return h