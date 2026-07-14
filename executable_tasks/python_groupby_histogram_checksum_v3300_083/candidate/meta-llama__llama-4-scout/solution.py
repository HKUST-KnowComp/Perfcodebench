import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        hist = np.zeros(buckets, dtype=np.int64)
        np.add(hist, vals[keys], hist)
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h