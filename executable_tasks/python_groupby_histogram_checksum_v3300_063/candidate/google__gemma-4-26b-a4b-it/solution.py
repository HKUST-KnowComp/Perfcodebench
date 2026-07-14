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
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
        
        # Vectorized checksum calculation
        # The original loop: h ^= v + 1; h = (h * prime) & mask
        # This is a sequential dependency, so we cannot fully vectorize the XOR-multiply chain
        # However, we can optimize the loop by using a local variable and minimizing lookups
        h = FNV_OFFSET_BASIS
        for v in hist:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
            
    return h