import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    ids_arr = np.asarray(ids, dtype=np.intp)
    vals_arr = np.asarray(dict_vals, dtype=np.uint64)
    
    # Pre-decode once - the decoded values don't change between iterations
    decoded = vals_arr[ids_arr]
    decoded_plus_one = decoded + np.uint64(1)
    
    # FNV-1a style hash constants
    MULT = np.uint64(1099511628211)
    INIT = np.uint64(1469598103934665603)
    
    # Compute hash for one iteration
    h = INIT
    for v in decoded_plus_one:
        h = (h ^ v) * MULT
    
    # For multiple iterations, we need to repeat the hash computation
    # But since decoded doesn't change, we can compute once and repeat
    if iters <= 1:
        return int(h)
    
    # For iters > 1, we need to recompute each time starting from INIT
    # Cache the decoded_plus_one as a list for faster iteration
    vals_list = decoded_plus_one.tolist()
    
    for _ in range(iters - 1):
        h = INIT
        for v in vals_list:
            h = ((h ^ v) * MULT) & 0xFFFFFFFFFFFFFFFF
    
    return int(h)