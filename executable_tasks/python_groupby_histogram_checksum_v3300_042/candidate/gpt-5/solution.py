def run(keys, vals, buckets: int, iters: int) -> int:
    # Build the histogram once
    hist = [0] * buckets
    k = keys
    v = vals
    n = len(k)
    i = 0
    while i < n:
        hist[k[i]] += v[i]
        i += 1

    # Compute checksum (FNV-1a style over the histogram)
    h = 1469598103934665603
    fnv_prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    for x in hist:
        h ^= x + 1
        h = (h * fnv_prime) & mask
    return h
