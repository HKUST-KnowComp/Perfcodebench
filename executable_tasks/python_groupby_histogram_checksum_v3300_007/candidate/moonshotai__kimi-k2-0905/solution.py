import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys = np.asarray(keys, dtype=np.intp)
    vals = np.asarray(vals, dtype=np.int64)

    # Pre-allocate once and reuse across iterations.
    hist = np.empty(buckets, dtype=np.int64)

    h = 0
    for _ in range(iters):
        # Fast vectorised histogram accumulation.
        hist.fill(0)
        np.add.at(hist, keys, vals)

        # FNV-1a 64-bit checksum over the bucket array.
        h = 1469598103934665603
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h