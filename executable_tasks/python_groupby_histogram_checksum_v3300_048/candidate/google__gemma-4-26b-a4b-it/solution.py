import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion overhead
    k_arr = np.array(keys, dtype=np.int64)
    v_arr = np.array(vals, dtype=np.int64)
    
    h = 0
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # Use bincount for high-performance dense histogram aggregation
        hist = np.bincount(k_arr, weights=v_arr, minlength=buckets).astype(np.int64)
        
        # Vectorized FNV-1a style checksum calculation
        # Note: Python integers handle arbitrary precision, but we must simulate 64-bit overflow
        h = 1469598103934665603
        
        # Since the checksum is sequential (h = (h ^ (v+1)) * prime), 
        # pure vectorization is tricky due to the dependency. 
        # However, we can use a loop over the array which is faster than the zip loop.
        # For very large buckets, we use the array directly.
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & mask
            
    return h