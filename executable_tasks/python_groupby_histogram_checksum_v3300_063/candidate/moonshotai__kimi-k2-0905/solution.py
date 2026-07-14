import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys = np.asarray(keys, dtype=np.intp)
    vals = np.asarray(vals, dtype=np.int64)
    h_out = np.zeros(1, dtype=np.uint64)

    # Compiled ufunc that computes FNV-1a checksum over an int64 array
    @np.vectorize(["uint64(uint64[:])"], target="cpu", nopython=True)
    def _fnv(hist):
        h = np.uint64(1469598103934665603)
        for v in hist:
            h ^= np.uint64(v + 1)
            h *= np.uint64(1099511628211)
        return h

    for _ in range(iters):
        hist = np.bincount(keys, weights=vals, minlength=buckets).astype(np.int64)
        h_out[0] = _fnv(hist)
    return int(h_out[0])