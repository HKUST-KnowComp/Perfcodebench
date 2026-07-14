def run(keys, vals, buckets: int, iters: int) -> int:
    # Build the histogram once; it is identical across iterations.
    hist = [0] * buckets

    n = len(keys)
    m = len(vals)
    cnt = n if n < m else m  # preserve zip truncation semantics

    hk = keys
    hv = vals
    hlist = hist

    i = 0
    while i < cnt:
        k = hk[i]
        hlist[k] += hv[i]
        i += 1

    # Compute FNV-1a-like checksum over the histogram once
    h = 1469598103934665603
    mul = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    for v in hlist:
        h ^= v + 1
        h = (h * mul) & mask

    return h
