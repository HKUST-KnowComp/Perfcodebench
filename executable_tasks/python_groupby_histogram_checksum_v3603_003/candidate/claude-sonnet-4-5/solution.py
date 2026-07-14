import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys_arr = np.asarray(keys, dtype=np.int32)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    for _ in range(iters - 1):
        np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
    
    hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
    
    h = 1469598103934665603
    for v in hist:
        h ^= int(v) + 1
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
