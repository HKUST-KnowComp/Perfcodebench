def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    pairs = list(zip(keys, vals))

    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for i in range(n):
            k, v = pairs[i]
            hist[k] += v

        h = offset
        for v in hist:
            h = ((h ^ (v + 1)) * prime) & mask
    return h
