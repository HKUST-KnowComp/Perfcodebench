def run(keys, vals, buckets: int, iters: int) -> int:
    if iters == 0:
        return 0
    # Build histogram
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v
    # FNV-1a 64-bit hash over histogram buckets
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    for v in hist:
        h = ((h ^ (v + 1)) * prime) & mask
    return h