def run(ids, dict_vals, iters: int) -> int:
    # Localize variables for faster access
    local_ids = ids
    local_vals = dict_vals
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    # The original code updates 'decoded' in every iteration, 
    # but the checksum is only calculated based on the state after the update.
    # We can merge the update and checksum steps to avoid the intermediate list.
    
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for idx in local_ids:
            v = local_vals[idx]
            h ^= v + 1
            h = (h * multiplier) & mask
    return h