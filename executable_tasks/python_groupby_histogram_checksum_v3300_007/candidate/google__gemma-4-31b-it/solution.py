import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    keys_arr = np.array(keys, dtype=np.int64)
    vals_arr = np.array(vals, dtype=np.int64)
    
    # Pre-calculate constants for the checksum
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    h_final = 0
    for _ in range(iters):
        # Use np.bincount for extremely fast dense histogram aggregation
        # weights=vals_arr allows summing the values instead of just counting occurrences
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        
        # The checksum loop is the remaining bottleneck. 
        # We iterate over the numpy array which is faster than a Python list.
        h = FNV_OFFSET_BASIS
        for v in hist:
            # Cast to int to ensure we are working with Python's arbitrary precision ints
            # for the checksum logic, though the mask keeps it to 64-bit.
            h ^= int(v) + 1
            h = (h * FNV_PRIME) & MASK
        h_final = h
        
    return h_final