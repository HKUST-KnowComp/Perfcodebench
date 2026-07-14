def run(keys, vals, buckets: int, iters: int) -> int:
    # Build histogram once
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v

    # Checksum constants
    h0 = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1

    h = 0
    for _ in range(iters):
        h = h0
        for v in hist:
            h ^= v + 1
            h = (h * mult) & mask
    return h
