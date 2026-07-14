import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    # Convert to numpy arrays if not already
    if not isinstance(ids, np.ndarray):
        ids = np.array(ids, dtype=np.intp)
    if not isinstance(dict_vals, np.ndarray):
        dict_vals = np.array(dict_vals, dtype=np.int64)
    
    # Vectorized decode using advanced indexing
    decoded = dict_vals[ids]
    
    # Pre-compute v + 1 values
    decoded_plus_one = decoded + 1
    
    # Constants
    INIT = np.uint64(1469598103934665603)
    MULT = np.uint64(1099511628211)
    MASK = np.uint64((1 << 64) - 1)
    
    h = np.uint64(0)
    
    # Main iteration loop
    for _ in range(iters):
        h = INIT
        for v in decoded_plus_one:
            h ^= np.uint64(v)
            h = (h * MULT) & MASK
    
    return int(h)