def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    hist = [0] * buckets
    n = len(keys)
    h = 0

    for _ in range(iters):
        # Reset histogram in-place to avoid repeated allocation.
        for i in range(buckets):
            hist[i] = 0

        # Aggregate using indexed access to avoid zip iterator overhead.
        for i in range(n):
            hist[keys[i]] += vals[i]

        # FNV-like checksum over buckets.
        h = offset
        for i in range(buckets):
            h ^= hist[i] + 1
            h = (h * prime) & mask

    return h
