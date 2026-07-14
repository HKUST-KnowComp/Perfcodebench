def run(keys, vals, buckets: int, iters: int) -> int:
    # If no iterations are requested, baseline returns 0
    if iters == 0:
        return 0

    # Build histogram once since each iteration performs identical work
    hist = [0] * buckets

    # Fast path for sequences supporting len and indexing
    if hasattr(keys, '__len__') and hasattr(keys, '__getitem__') and hasattr(vals, '__len__') and hasattr(vals, '__getitem__'):
        n = len(keys)
        m = len(vals)
        if m < n:
            n = m
        hlist = hist
        kseq = keys
        vseq = vals
        for i in range(n):
            hlist[kseq[i]] += vseq[i]
    else:
        # Fallback for generic iterables
        hlist = hist
        for k, v in zip(keys, vals):
            hlist[k] += v

    # Compute FNV-1a-like checksum over buckets
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211
    for val in hist:
        h ^= val + 1
        h = (h * mul) & mask

    return h
