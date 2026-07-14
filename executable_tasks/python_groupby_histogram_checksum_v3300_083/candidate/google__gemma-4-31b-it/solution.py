import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion overhead
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    h = 0
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # np.bincount is significantly faster than a Python loop for histogramming
        # minlength ensures the histogram has exactly 'buckets' elements
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        
        # The checksum must be calculated exactly as per the baseline
        h = 1469598103934665603
        for v in hist:
            # Cast to int to ensure we are dealing with Python ints for the checksum
            h ^= int(v) + 1
            h = (h * 1099511628211) & mask
            
    return h