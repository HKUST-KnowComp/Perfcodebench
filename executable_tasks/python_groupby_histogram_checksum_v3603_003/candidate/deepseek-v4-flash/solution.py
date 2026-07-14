def run(keys, vals, buckets: int, iters: int) -> int:
    # Build histogram once (keys and vals are unchanged across iterations)
    hist = [0] * buckets
    n = len(keys)
    for i in range(n):
        hist[keys[i]] += vals[i]

    # Constants for FNV-1a 64-bit hash
    h_start = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1

    h = h_start
    # Repeat checksum computation for each iteration
    for _ in range(iters):
        h = h_start
        for v in hist:
            h ^= v + 1
            h = (h * mult) & mask
    return h