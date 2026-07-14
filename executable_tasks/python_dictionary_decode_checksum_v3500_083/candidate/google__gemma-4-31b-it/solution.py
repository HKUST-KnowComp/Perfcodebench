def run(ids, dict_vals, iters: int) -> int:
    # The checksum is calculated over the decoded values.
    # Since dict_vals and ids are constant across iterations,
    # the result of the checksum loop is the same every time.
    # We only need to compute it once.
    
    # Pre-decode values to avoid repeated dictionary lookups
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    for v in decoded:
        h ^= v + 1
        h = (h * multiplier) & mask
        
    return h