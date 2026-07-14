def run(ids, dict_vals, iters: int) -> int:
    # The original code updates 'decoded' iters times, but the final 'h' 
    # is calculated only after the last iteration of the 'iters' loop.
    # Since the values in 'decoded' are identical in every iteration,
    # we only need to perform the lookup and hashing once.
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    prime = 1099511628211
    
    # Pre-fetch dict_vals lookup to avoid repeated attribute access
    # and iterate directly over ids to avoid enumerate overhead.
    for idx in ids:
        v = dict_vals[idx]
        h ^= v + 1
        h = (h * prime) & mask
        
    return h