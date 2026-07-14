def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    kv = list(zip(keys, vals))

    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for i in range(n):
            k, v = kv[i]
            hist[k] += v

        hh = offset
        for i in range(buckets):
            hh ^= hist[i] + 1
            hh = (hh * prime) & mask
        h = hh
    return h
