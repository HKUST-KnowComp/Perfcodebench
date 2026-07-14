def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211

    n = len(keys)
    hist = [0] * buckets
    touched = []
    touched_append = touched.append

    for i in range(n):
        k = keys[i]
        if hist[k] == 0:
            touched_append(k)
        hist[k] += vals[i]

    h = fnv_offset
    for v in hist:
        h ^= v + 1
        h = (h * fnv_prime) & mask

    for _ in range(iters - 1):
        for k in touched:
            hist[k] = 0
        for i in range(n):
            hist[keys[i]] += vals[i]
        h = fnv_offset
        for v in hist:
            h ^= v + 1
            h = (h * fnv_prime) & mask

    return h
