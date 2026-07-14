def run(keys, vals, buckets: int, iters: int) -> int:
    try:
        import numpy as np
        keys_arr = np.asarray(keys, dtype=np.intp)
        vals_arr = np.asarray(vals, dtype=np.int64)
        hist_arr = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        hist = hist_arr.astype(np.int64).tolist()
    except Exception:
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
    
    mask = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & mask
    return h
