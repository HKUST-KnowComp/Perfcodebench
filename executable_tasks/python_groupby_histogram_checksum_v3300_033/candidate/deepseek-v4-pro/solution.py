def run(keys, vals, buckets: int, iters: int) -> int:
    if iters <= 0:
        return 0
    # Only the final iteration affects the result (each loop resets h).
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for v in hist:
        h ^= (v + 1)
        h = (h * mult) & mask
    return h
