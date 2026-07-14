import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys = np.asarray(keys, dtype=np.intp)
    vals = np.asarray(vals, dtype=np.intp)

    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK_64 = (1 << 64) - 1

    h = 0
    for _ in range(iters):
        hist = np.zeros(buckets, dtype=np.intp)
        np.add.at(hist, keys, vals)

        h = FNV_OFFSET_BASIS
        for v in hist:
            h ^= (int(v) + 1)
            h = (h * FNV_PRIME) & MASK_64
    return h