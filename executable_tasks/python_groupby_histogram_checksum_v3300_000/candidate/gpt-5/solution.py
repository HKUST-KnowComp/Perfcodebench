def run(keys, vals, buckets: int, iters: int) -> int:
    # Prebind constants and frequently used references
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603

    k = keys
    v = vals
    count = min(len(k), len(v))

    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        hl = hist  # local alias for speed

        # Index-based accumulation avoids zip's tuple creation per iteration
        for i in range(count):
            hl[k[i]] += v[i]

        # FNV-1a style checksum over buckets
        xh = offset
        for x in hl:
            xh ^= x + 1
            xh = (xh * prime) & mask
        h = xh

    return h
