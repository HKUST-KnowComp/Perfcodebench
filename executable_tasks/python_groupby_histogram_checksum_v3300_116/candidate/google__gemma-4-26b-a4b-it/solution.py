import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys_arr = np.array(keys, dtype=np.int64)
    vals_arr = np.array(vals, dtype=np.int64)
    h = 0
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # Use bincount for high-performance dense aggregation
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        # Vectorized FNV-1a style checksum calculation
        # Note: The original loop is: h = 1469598103934665603; for v in hist: h ^= v + 1; h = (h * 1099511628211) & mask
        # Since the loop is sequential and depends on the previous h, we must maintain order.
        # However, for the purpose of this specific task, we can optimize the loop itself.
        h = 1469598103934665603
        for v in hist:
            h = ((h ^ (v + 1)) * 1099511628211) & mask
            
    return h