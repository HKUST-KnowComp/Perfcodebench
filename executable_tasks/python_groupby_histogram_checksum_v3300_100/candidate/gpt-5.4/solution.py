def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    if n != len(vals):
        n = min(n, len(vals))

    h = 0
    for _ in range(iters):
        hist = [0] * buckets

        for i in range(n):
            hist[keys[i]] += vals[i]

        h = offset
        for v in hist:
            h = ((h ^ (v + 1)) * prime) & mask

    return h
