import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys = np.asarray(keys, dtype=np.intp)
    vals = np.asarray(vals, dtype=np.int64)
    h_out = 0

    # Compiled ufunc for the FNV-1a checksum over an int64 array
    def fnv1a(arr):
        h = np.uint64(14695981039346656037)
        prime = np.uint64(1099511628211)
        for v in arr:
            h ^= np.uint64(v + 1)
            h *= prime
        return int(h)

    for _ in range(iters):
        hist = np.bincount(keys, weights=vals, minlength=buckets).astype(np.int64)
        h_out = fnv1a(hist)
    return h_out