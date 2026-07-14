def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211

    n = len(keys)
    zero_hist = [0] * buckets
    h = 0

    for _ in range(iters):
        hist = zero_hist.copy()

        hk = hist
        ks = keys
        vs = vals
        for i in range(n):
            hk[ks[i]] += vs[i]

        hh = fnv_offset
        for v in hk:
            hh ^= v + 1
            hh = (hh * fnv_prime) & mask
        h = hh

    return h
