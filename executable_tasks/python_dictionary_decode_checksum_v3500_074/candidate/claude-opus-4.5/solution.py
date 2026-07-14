import numpy as np
try:
    from numba import njit
    HAVE_NUMBA = True
except ImportError:
    HAVE_NUMBA = False

if HAVE_NUMBA:
    @njit(cache=True)
    def _hash_loop(decoded, iters):
        n = len(decoded)
        h = 0
        for _ in range(iters):
            h = 1469598103934665603
            for i in range(n):
                h ^= decoded[i] + 1
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        return h

    def run(ids, dict_vals, iters: int) -> int:
        ids_arr = np.asarray(ids, dtype=np.int64)
        vals_arr = np.asarray(dict_vals, dtype=np.int64)
        decoded = vals_arr[ids_arr]
        return _hash_loop(decoded, iters)
else:
    def run(ids, dict_vals, iters: int) -> int:
        decoded = [dict_vals[idx] for idx in ids]
        h = 0
        for _ in range(iters):
            h = 1469598103934665603
            for v in decoded:
                h ^= v + 1
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        return h
