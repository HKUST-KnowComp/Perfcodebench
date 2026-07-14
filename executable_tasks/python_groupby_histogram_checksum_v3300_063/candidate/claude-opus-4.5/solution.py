def run(keys, vals, buckets: int, iters: int) -> int:
    import numpy as np
    
    # Convert to numpy arrays if not already
    keys_arr = np.asarray(keys, dtype=np.intp)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    # Compute histogram once using vectorized bincount
    hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
    
    # Ensure we have exactly 'buckets' elements
    if len(hist) > buckets:
        hist = hist[:buckets]
    
    # Convert to list for checksum computation
    hist_list = hist.tolist()
    
    # Compute checksum for required iterations
    mask = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for v in hist_list:
            h ^= (v + 1) & mask
            h = (h * 1099511628211) & mask
    
    return h
