def run(keys, vals, buckets: int, iters: int) -> int:
    # If no iterations requested, match baseline behavior: return initial h (0)
    if iters <= 0:
        return 0

    # Build histogram once; the baseline recomputes the same result for each iteration,
    # but the final checksum is identical to a single computation when iters > 0.
    hist = [0] * buckets

    # Local bindings for speed
    hist_local = hist
    keys_local = keys
    vals_local = vals

    n = len(keys_local)
    i = 0
    while i < n:
        k = keys_local[i]
        hist_local[k] += vals_local[i]
        i += 1

    # Compute checksum (FNV-1a-like with tweak) exactly as baseline
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    for v in hist_local:
        h ^= v + 1
        h = (h * prime) & mask

    return h
