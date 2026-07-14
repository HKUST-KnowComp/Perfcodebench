def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    hist = [0] * buckets

    for _ in range(iters):
        # Reset histogram in-place to avoid reallocating the list each iteration.
        for i in range(buckets):
            hist[i] = 0

        # Index-based loop is typically faster than zip() iteration for tight loops.
        for i in range(n):
            hist[keys[i]] += vals[i]

        h = offset
        for i in range(buckets):
            h ^= hist[i] + 1
            h = (h * prime) & mask

    return h
