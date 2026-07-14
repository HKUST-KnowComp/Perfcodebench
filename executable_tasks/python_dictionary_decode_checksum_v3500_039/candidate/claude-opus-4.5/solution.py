import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    ids_arr = np.asarray(ids, dtype=np.int64)
    dict_arr = np.asarray(dict_vals, dtype=np.int64)
    
    # Decode once - the decoded values don't change across iterations
    decoded = dict_arr[ids_arr]
    
    # Precompute (v + 1) values
    vals_plus_one = decoded + 1
    
    # Constants for FNV-like hash
    MULT = np.uint64(1099511628211)
    MASK = np.uint64((1 << 64) - 1)
    
    # The hash computation is: h = ((h ^ (v+1)) * MULT) & MASK
    # We need to compute this sequentially, but we can use numpy scalar ops
    # which are still faster than pure Python
    
    h = np.uint64(0)
    
    for _ in range(iters):
        h = np.uint64(1469598103934665603)
        for v in vals_plus_one:
            h = ((h ^ np.uint64(v)) * MULT) & MASK
    
    return int(h)
