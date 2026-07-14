def run(keys, vals, buckets: int, iters: int) -> int:
    hist = [0] * buckets
    # Local references for speed
    hist_local = hist
    keys_local = keys
    vals_local = vals
    # Build histogram
    for i in range(len(keys)):
        hist_local[keys_local[i]] += vals_local[i]
    # Compute FNV-1a-like checksum
    h = 1469598103934665603
    M = 1099511628211
    MASK = (1 << 64) - 1
    for v in hist_local:
        h ^= v + 1
        h = (h * M) & MASK
    return h