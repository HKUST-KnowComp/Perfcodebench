import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    keys_arr = np.array(keys, dtype=np.int64)
    vals_arr = np.array(vals, dtype=np.int64)
    
    h = 0
    mask = (1 << 64) - 1
    mult = 1099511628211
    
    for _ in range(iters):
        # np.bincount is highly optimized for this exact operation
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * mult) & mask
            
    return h