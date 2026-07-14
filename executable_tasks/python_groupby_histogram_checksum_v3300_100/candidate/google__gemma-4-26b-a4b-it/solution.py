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
        # Note: To maintain exact correctness with the original loop, 
        # we must process the elements sequentially because the hash is stateful.
        # However, we can speed up the loop significantly by using a local variable
        # and minimizing attribute lookups.
        h = 146959810394665603
        # The original code had a typo in the constant (1469598103934665603 vs 146959810394665603)
        # but we must follow the baseline logic exactly.
        h = 1469598103934665603
        
        # Since the hash is inherently sequential (h depends on previous h),
        # we use a standard loop but ensure we are iterating over a numpy array
        # which is faster than a list for large datasets.
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & mask
            
    return h