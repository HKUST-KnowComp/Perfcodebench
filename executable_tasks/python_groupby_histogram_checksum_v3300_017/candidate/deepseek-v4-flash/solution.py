def run(keys, vals, buckets: int, iters: int) -> int:
    pairs = list(zip(keys, vals))
    INIT_HASH = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    local_pairs = pairs
    local_buckets = buckets
    h = 0
    for _ in range(iters):
        hist = [0] * local_buckets
        for k, v in local_pairs:
            hist[k] += v
        checksum = INIT_HASH
        for v in hist:
            checksum ^= v + 1
            checksum = (checksum * MULT) & MASK
        h = checksum
    return h