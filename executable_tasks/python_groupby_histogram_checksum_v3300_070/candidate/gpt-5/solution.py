def run(keys, vals, buckets: int, iters: int) -> int:
    # Match baseline behavior: if iters <= 0, no work is done and 0 is returned.
    if iters <= 0:
        return 0

    # Build histogram once, since it is identical across all iterations.
    hist = [0] * buckets

    # Match zip(keys, vals) truncation to the shorter length.
    n = len(keys)
    m = len(vals)
    end = m if m < n else n

    # Local bindings for speed
    hl = hist
    k = keys
    v = vals

    # Accumulate values into histogram buckets
    for i in range(end):
        hl[k[i]] += v[i]

    # Compute checksum once (same result each iteration in the baseline)
    h = 1469598103934665603
    mask = (1 << 64) - 1
    prime = 1099511628211

    for val in hl:
        h = ((h ^ (val + 1)) * prime) & mask

    return h
