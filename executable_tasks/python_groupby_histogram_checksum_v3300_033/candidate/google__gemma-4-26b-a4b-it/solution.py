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
        hist = np.bincount(k_arr, weights=v_arr, minlength=buckets).astype(np.uint64)
        
        # Vectorized checksum calculation is difficult for FNV-1a due to its sequential nature.
        # However, we can use a standard loop over the numpy array which is faster than a list.
        h = FNV_OFFSET_BASIS
        for v in hist:
            h = (h ^ (v + 1)) & MASK64
            h = (h * FNV_PRIME) & MASK64
            
    return h