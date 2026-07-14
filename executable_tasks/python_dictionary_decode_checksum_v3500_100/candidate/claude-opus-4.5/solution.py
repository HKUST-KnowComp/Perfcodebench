import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    ids_arr = np.asarray(ids, dtype=np.int64)
    dict_vals_arr = np.asarray(dict_vals, dtype=np.int64)
    
    # Decode once - the mapping doesn't change between iterations
    decoded = dict_vals_arr[ids_arr]
    
    # Pre-compute (v + 1) values since they don't change
    decoded_plus_one = (decoded + 1).astype(np.uint64)
    
    # FNV-1a constants
    FNV_PRIME = np.uint64(1099511628211)
    FNV_OFFSET = np.uint64(1469598103934665603)
    MASK = np.uint64((1 << 64) - 1)
    
    # Compute hash for one iteration
    # The hash computation is sequential, but we can optimize the inner loop
    h = np.uint64(0)
    for _ in range(iters):
        h = FNV_OFFSET
        for v in decoded_plus_one:
            h ^= v
            h = (h * FNV_PRIME) & MASK
    
    return int(h)