def run(ids, dict_vals, iters: int) -> int:
    # Pre-calculate the decoded values for the first iteration to avoid repeated allocation
    # and use a local variable for the mask to speed up access.
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    # To optimize, we can pre-fetch the values from dict_vals into a list
    # Since the values in dict_vals don't change, and ids don't change,
    # the 'decoded' list will be identical in every iteration.
    # The original code updates 'decoded' every iteration, but the values are constant.
    # Therefore, we only need to compute the decoded list once.
    
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for v in decoded:
            h ^= v + 1
            h = (h * multiplier) & mask
            
    return h