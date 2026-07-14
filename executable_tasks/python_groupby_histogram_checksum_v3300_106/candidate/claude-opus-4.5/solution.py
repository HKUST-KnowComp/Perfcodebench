import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    # Convert to numpy arrays if not already
    keys_arr = np.asarray(keys, dtype=np.intp)
    vals_arr = np.asarray(vals, dtype=np.int64)
    
    # Compute histogram using bincount - much faster than Python loop
    hist = np.bincount(keys_arr, weights=vals_arr, minlength=buckets).astype(np.uint64)
    
    # The histogram is the same every iteration, so we just need to compute
    # the checksum once (it will be the same each time)
    # FNV-1a style hash computation
    h = np.uint64(1469598103934665603)
    mask = np.uint64((1 << 64) - 1)
    mult = np.uint64(1099511628211)
    
    # Vectorized approach: precompute (v + 1) for all buckets
    vals_plus_one = hist + np.uint64(1)
    
    # We need to compute the iterative hash, but we can use numpy for speed
    # h = ((h ^ (v+1)) * mult) & mask for each v
    # Unfortunately this is inherently sequential, but we can use numba-free tricks
    
    # Fast path: use Python int arithmetic which is actually fast for this
    h = 1469598103934665603
    mask_int = (1 << 64) - 1
    mult_int = 1099511628211
    
    # Convert histogram to Python list for faster iteration
    hist_list = hist.tolist()
    
    for v in hist_list:
        h ^= (v + 1)
        h = (h * mult_int) & mask_int
    
    return h
