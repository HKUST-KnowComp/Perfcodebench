def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    hist = [0] * buckets

    for _ in range(iters):
        if buckets:
            hist[:] = [0] * buckets

        for i in range(n):
            hist[keys[i]] += vals[i]

        h = offset
        for i in range(buckets):
            h ^= hist[i] + 1
            h = (h * prime) & mask

    return h
