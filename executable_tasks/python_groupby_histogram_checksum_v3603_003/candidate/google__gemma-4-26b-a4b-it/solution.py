import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion overhead
    k_arr = np.asarray(keys, dtype=np.int64)
    v_arr = np.asarray(vals, dtype=np.int64)
    
    h = 0
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # Use bincount for high-performance dense histogram aggregation
        hist = np.bincount(k_arr, weights=v_arr, minlength=buckets).astype(np.int64)
        
        # Vectorized FNV-1a style checksum calculation
        # We must simulate the sequential nature of the hash: h = (h ^ (v + 1)) * prime
        # Since the hash is sequential, we cannot fully vectorize the XOR/Multiply chain
        # without a custom C extension, but we can optimize the loop significantly.
        h = 1469598103934665603
        for v in hist:
            h = ((h ^ (v + 1)) * 1099511628211) & mask
            
    return h