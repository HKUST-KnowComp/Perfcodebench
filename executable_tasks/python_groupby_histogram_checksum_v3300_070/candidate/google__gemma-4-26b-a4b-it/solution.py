import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion overhead
    k_arr = np.array(keys, dtype=np.int64)
    v_arr = np.array(vals, dtype=np.int64)
    
    h = 0
    # Pre-calculate constants for the FNV-1a style hash
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    for _ in range(iters):
        # Use bincount for high-performance dense histogram aggregation
        # minlength ensures the array is exactly 'buckets' long
        hist = np.bincount(k_arr, weights=v_arr, minlength=buckets).astype(np.int64)
        
        # Vectorized checksum calculation
        # We must simulate the sequential nature of the hash: h = (h ^ (v + 1)) * prime
        # Since the hash is sequential, we cannot fully vectorize the dependency 
        # without a custom C extension, but we can optimize the loop significantly.
        # However, for the given constraints, a tight loop over the numpy array 
        # is faster than the original zip/list approach.
        
        h = FNV_OFFSET_BASIS
        # Accessing the underlying buffer directly via a loop is faster than list iteration
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK64
            
    return h