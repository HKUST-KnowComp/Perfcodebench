import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    # Convert to numpy arrays for vectorized operations
    ids_arr = np.asarray(ids, dtype=np.int64)
    dict_arr = np.asarray(dict_vals, dtype=np.int64)
    
    # Decode once - the mapping doesn't change between iterations
    decoded = dict_arr[ids_arr]
    
    # Precompute (v + 1) values
    vals_plus_one = decoded + 1
    
    # The hash computation: h = ((h ^ (v+1)) * multiplier) & mask
    # We need to do this sequentially due to data dependency
    # But we can use numpy scalar operations which are faster
    
    multiplier = np.uint64(1099511628211)
    mask = np.uint64((1 << 64) - 1)
    
    # Convert to uint64 for proper overflow behavior
    vals_u64 = vals_plus_one.astype(np.uint64)
    
    h = np.uint64(0)
    for _ in range(iters):
        h = np.uint64(1469598103934665603)
        for v in vals_u64:
            h = ((h ^ v) * multiplier) & mask
    
    return int(h)
