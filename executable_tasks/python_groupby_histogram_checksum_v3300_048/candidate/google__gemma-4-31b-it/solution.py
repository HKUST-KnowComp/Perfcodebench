import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays for fast processing
    keys_arr = np.array(keys, dtype=np.int64)
    vals_arr = np.array(vals, dtype=np.int64)
    
    # Constants for the checksum
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    h_final = 0
    for _ in range(iters):
        # Use np.bincount for fast weighted histogram aggregation
        # minlength ensures the histogram has exactly 'buckets' elements
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        # The checksum is a sequential dependency, so we must loop.
        # However, we operate on the numpy array which is faster than a Python list.
        h = FNV_OFFSET_BASIS
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
        h_final = h
        
    return h_final