import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert to numpy arrays once if not already
    keys_arr = np.asarray(keys, dtype=np.int32)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    h = 0
    for _ in range(iters):
        # Use numpy bincount for fast histogram - vectorized C implementation
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        
        # Vectorized checksum computation
        h = np.uint64(1469598103934665603)
        
        # Compute all XOR and multiply operations in vectorized fashion
        hist_plus_1 = hist + 1
        
        # Manual loop is still needed for the sequential hash dependency
        # but we work with numpy array which is faster
        for v in hist_plus_1:
            h ^= np.uint64(v)
            h = (h * np.uint64(1099511628211)) & np.uint64(0xFFFFFFFFFFFFFFFF)
        
        h = int(h)
    
    return h