import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys = np.asarray(keys, dtype=np.int32)
    vals = np.asarray(vals, dtype=np.int32)
    h = 0
    for _ in range(iters):
        hist = np.bincount(keys, weights=vals, minlength=buckets)
        h = 1469598103934665603
        h ^= (hist + 1).dot(1099511628211 ** np.arange(len(hist), dtype=np.uint64) + 1) & ((1 << 64) - 1)
    return h