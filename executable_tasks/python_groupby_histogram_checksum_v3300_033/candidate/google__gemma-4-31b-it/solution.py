import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays for vectorized operations
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    # Pre-calculate constants for the checksum
    MOD = (1 << 64)
    MULT = 1099511628211
    INIT_H = 1469598103934665603

    h = 0
    for _ in range(iters):
        # Use np.bincount for fast histogram aggregation
        # weights=vals_arr allows summing the values instead of just counting
        # minlength ensures the histogram has exactly 'buckets' elements
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        # The checksum loop is the second bottleneck. 
        # Since it's a sequential hash, we must iterate, but we can optimize the loop
        # by using a local variable for h and avoiding repeated attribute lookups.
        curr_h = INIT_H
        for v in hist:
            curr_h = ((curr_h ^ (v + 1)) * MULT) & (MOD - 1)
        h = curr_h
        
    return h