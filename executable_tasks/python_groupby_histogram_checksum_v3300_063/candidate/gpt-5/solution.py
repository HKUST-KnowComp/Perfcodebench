def run(keys, vals, buckets: int, iters: int) -> int:
    # Preserve baseline behavior: if iters == 0, return 0
    if iters <= 0:
        return 0

    # Build the histogram once
    hist = [0] * buckets
    k = keys
    v = vals
    hk = hist
    n = len(k)
    m = len(v)
    cnt = n if n < m else m  # min(len(keys), len(vals)) to match zip semantics

    i = 0
    while i < cnt:
        hk[k[i]] += v[i]
        i += 1

    # Hash once (FNV-1-like), matching baseline masking behavior
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mul = 1099511628211
    for x in hk:
        h ^= x + 1
        h = (h * mul) & mask

    return h
