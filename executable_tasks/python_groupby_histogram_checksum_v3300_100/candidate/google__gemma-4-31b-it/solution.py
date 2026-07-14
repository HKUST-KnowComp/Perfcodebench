import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays for fast processing
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    h_const = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # Use np.bincount for high-performance aggregation
        # weights=vals_arr allows summing the values instead of just counting
        # minlength ensures the histogram has exactly 'buckets' size
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        
        # The checksum loop must be performed in Python to maintain the 64-bit overflow behavior
        # and the specific hash algorithm provided in the baseline.
        h = h_const
        for v in hist:
            # Cast to int to ensure we are working with Python's arbitrary precision ints
            # before applying the 64-bit mask
            h ^= int(v) + 1
            h = (h * mult) & mask
            
    return h