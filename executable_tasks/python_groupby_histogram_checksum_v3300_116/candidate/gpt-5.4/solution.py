def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    hist = [0] * buckets
    touched = []

    for _ in range(iters):
        touched.clear()

        for i in range(n):
            k = keys[i]
            if hist[k] == 0:
                touched.append(k)
            hist[k] += vals[i]

        h = offset
        for v in hist:
            h ^= v + 1
            h = (h * prime) & mask

        for k in touched:
            hist[k] = 0

    return h
