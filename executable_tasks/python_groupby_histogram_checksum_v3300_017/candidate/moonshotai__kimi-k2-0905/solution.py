import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys_arr = np.asarray(keys, dtype=np.intp)
    vals_arr = np.asarray(vals, dtype=np.int64)
    hist = np.zeros(buckets, dtype=np.int64)

    for _ in range(iters):
        hist.fill(0)
        np.add.at(hist, keys_arr, vals_arr)

        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h