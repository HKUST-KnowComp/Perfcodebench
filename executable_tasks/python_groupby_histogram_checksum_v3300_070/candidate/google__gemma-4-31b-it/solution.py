import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert inputs to numpy arrays once to avoid repeated conversion overhead
    keys_arr = np.asarray(keys, dtype=np.int64)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    mask = (1 << 64) - 1
    h_final = 0
    
    for _ in range(iters):
        # np.bincount is significantly faster than a Python loop for dense histograms
        # minlength ensures the output array has exactly 'buckets' elements
        hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets)
        
        # The checksum must be calculated on the resulting histogram
        # We cast to int64 to ensure precision and avoid float issues from bincount weights
        hist_int = hist.astype(np.int64)
        
        h = 1469598103934665603
        for v in hist_int:
            h ^= v + 1
            h = (h * 1099511628211) & mask
        h_final = h
        
    return h_final