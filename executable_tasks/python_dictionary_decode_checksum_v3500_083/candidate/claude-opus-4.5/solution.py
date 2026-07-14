import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    ids_arr = np.asarray(ids, dtype=np.intp)
    dict_arr = np.asarray(dict_vals, dtype=np.int64)
    
    # Decode once - the decoded values don't change across iterations
    decoded = dict_arr[ids_arr]
    
    # Precompute (v + 1) values
    v_plus_1 = (decoded.astype(np.uint64) + np.uint64(1))
    
    # FNV-like hash constants
    MULT = np.uint64(1099511628211)
    MASK = np.uint64((1 << 64) - 1)
    
    h = np.uint64(0)
    for _ in range(iters):
        h = np.uint64(1469598103934665603)
        for v in v_plus_1:
            h ^= v
            h = (h * MULT) & MASK
    
    return int(h)
