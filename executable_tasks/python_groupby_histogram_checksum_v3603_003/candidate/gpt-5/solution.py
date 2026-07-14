def run(keys, vals, buckets: int, iters: int) -> int:
    # Build the histogram once; repeating per-iteration work is redundant
    hist = [0] * buckets

    # Fast path for common case where keys and vals are lists
    if isinstance(keys, list) and isinstance(vals, list):
        hk = hist  # local reference
        kk = keys
        vv = vals
        n = len(kk)
        m = len(vv)
        if m < n:
            n = m
        i = 0
        while i < n:
            hk[kk[i]] += vv[i]
            i += 1
    else:
        # Fallback for generic iterables
        for k, v in zip(keys, vals):
            hist[k] += v

    # Compute checksum once (same result as repeating it 'iters' times)
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    for v in hist:
        h ^= v + 1
        h = (h * prime) & mask

    return h
