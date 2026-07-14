def run(keys, vals, buckets: int, iters: int) -> int:
    # Match baseline behavior for iters == 0
    if iters == 0:
        return 0

    # Build histogram once (baseline rebuilt it per iteration but result only depends on one)
    hist = [0] * buckets
    hk = hist  # local alias for speed

    # Match zip(keys, vals) truncation behavior
    n = min(len(keys), len(vals))
    k = keys
    v = vals

    # Accumulate into histogram using indexed loop for performance
    for i in range(n):
        hk[k[i]] += v[i]

    # Compute the checksum (FNV-like), same as baseline
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211
    for x in hk:
        h ^= x + 1
        h = (h * mul) & mask

    return h
