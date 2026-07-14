import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys_arr = np.array(keys, dtype=np.int64)
    vals_arr = np.array(vals, dtype=np.int64)
    h = 0
    
    # Pre-calculate constants for the FNV-1a style hash
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for _ in range(iters):
        # Use numpy.bincount for high-performance dense aggregation
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.uint64)
        
        h = FNV_OFFSET_BASIS
        # Vectorized checksum calculation is difficult for FNV-1a due to sequential dependency.
        # However, we can optimize the loop by using a local variable and minimizing lookups.
        # For very large buckets, a pure Python loop is the bottleneck.
        # We use a standard loop but ensure we are working with native types.
        
        # Convert to list or use a more efficient iteration if possible
        # Given the constraints, a tight loop over the numpy array is the most reliable way
        # to maintain the exact sequential dependency of the FNV-1a hash.
        h_val = FNV_OFFSET_BASIS
        for v in hist:
            h_val ^= (int(v) + 1)
            h_val = (h_val * FNV_PRIME) & MASK
        h = h_val
        
    return h