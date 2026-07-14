import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert to numpy arrays if not already
    keys_arr = np.asarray(keys, dtype=np.intp)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    # Compute histogram using bincount - much faster than Python loop
    hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.int64)
    
    # The histogram is the same every iteration, so we only need to compute
    # the checksum once and return it (the loop just overwrites h each time)
    # But we need to run the iterations as specified for correctness
    
    # FNV-1a style hash must be computed sequentially due to data dependencies
    # We'll compute it in Python but only once per unique histogram
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        h = 1469598103934665603
        for v in hist:
            h ^= int(v) + 1
            h = (h * 1099511628211) & mask
    
    return h
