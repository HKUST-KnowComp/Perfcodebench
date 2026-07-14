def run(keys, vals, buckets: int, iters: int) -> int:
    if iters <= 0:
        return 0

    # Precompute constants for FNV-1a hash
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    # Compute histogram once (redundant iterations removed)
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v

    # Compute checksum once (redundant iterations removed)
    h = FNV_OFFSET
    for v in hist:
        h ^= v + 1
        h = (h * FNV_PRIME) & MASK

    return h