import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion in the loop
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    h = 0
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for _ in range(iters):
        # Use np.bincount for high-performance histogram aggregation
        # minlength ensures the result has exactly 'buckets' elements
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        # The checksum must be calculated sequentially as it is a hash
        # However, we can optimize the loop slightly
        current_h = 1469598103934665603
        for v in hist:
            current_h = ((current_h ^ (v + 1)) * multiplier) & mask
        h = current_h
        
    return h