def run(ids, dict_vals, iters: int) -> int:
    if not ids:
        # If no ids, the loop doesn't run, but the hash initialization logic 
        # implies we return the initial hash if iters > 0? 
        # Actually, the baseline returns 1469598103934665603 if iters > 0 and ids is empty.
        return 1469598103934665603 if iters > 0 else 0

    # The values in 'decoded' are constant across all iterations
    # because 'ids' and 'dict_vals' do not change.
    # We only need to compute the hash once.
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    # Pre-fetch values to avoid repeated dict lookups
    # and perform the hash calculation once.
    for idx in ids:
        v = dict_vals[idx]
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h