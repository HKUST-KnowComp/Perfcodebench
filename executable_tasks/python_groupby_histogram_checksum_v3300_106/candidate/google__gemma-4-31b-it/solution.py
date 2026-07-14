import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    mask64 = (1 << 64) - 1
    h_final = 0
    
    for _ in range(iters):
        # Use np.bincount for extremely fast histogram aggregation
        # minlength ensures the array is exactly 'buckets' long
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        h = 1469598103934665603
        # The checksum loop is the remaining bottleneck
        # We iterate over the numpy array which is faster than a Python list
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & mask64
        h_final = h
        
    return h_final