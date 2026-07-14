def run(keys, vals, buckets: int, iters: int) -> int:
    # Build histogram once
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v

    # Checksum parameters
    mask = 0xFFFFFFFFFFFFFFFF
    init = 1469598103934665603
    multiplier = 1099511628211

    # Outer loop for iters iterations
    h = 0
    for _ in range(iters):
        h = init
        for v in hist:
            h = (h ^ (v + 1)) * multiplier & mask
    return h
