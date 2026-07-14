import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion in the loop
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    h_const = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1

    for _ in range(iters):
        # np.bincount is significantly faster than a Python loop for histogramming
        # minlength ensures the result has exactly 'buckets' elements
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        
        h = h_const
        # The checksum loop must remain in Python or use a fast iterator
        # since it's a sequential dependency (FNV-like hash)
        for v in hist:
            # Cast to int to ensure we are dealing with Python's arbitrary precision
            # or fixed 64-bit logic as per the original implementation
            h ^= int(v) + 1
            h = (h * mult) & mask
            
    return h