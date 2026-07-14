import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion overhead
    k_arr = np.asarray(keys, dtype=np.int64)
    v_arr = np.asarray(vals, dtype=np.int64)
    
    h = 0
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # Use bincount for high-performance dense histogram aggregation
        hist = np.bincount(k_arr, weights=v_arr, minlength=buckets).astype(np.uint64)
        
        # Vectorized FNV-1a style checksum calculation
        # Note: Since the checksum is sequential (h = (h ^ (v+1)) * prime), 
        # we cannot fully vectorize the dependency, but we can use a loop 
        # over the array or a more efficient approach if the array is large.
        # However, for the specific FNV-1a pattern, a standard loop in Python 
        # is often the bottleneck. We use a tight loop here.
        
        h = 1469598103934665603
        for v in hist:
            h = ((h ^ (v + 1)) * 1099511628211) & mask
            
    return h