import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys_arr = np.array(keys, dtype=np.int64)
    vals_arr = np.array(vals, dtype=np.int64)
    h = 0
    
    # Pre-calculate constants for the FNV-1a style hash
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    for _ in range(iters):
        # Use numpy.bincount for high-performance dense aggregation
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.uint64)
        
        # Vectorized checksum calculation
        # The original loop: h = (h ^ (v + 1)) * prime
        # We must process elements sequentially to maintain correctness of the FNV-1a hash
        # However, we can use a Numba-like approach or simply optimize the loop if Numba isn't available.
        # Since we only have numpy, we use a standard loop but the bottleneck (aggregation) is solved.
        
        h = FNV_OFFSET_BASIS
        # Note: The checksum is order-dependent. We must iterate through the array.
        # For large buckets, this loop is the new bottleneck, but bincount is the primary gain.
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK64
            
    return h