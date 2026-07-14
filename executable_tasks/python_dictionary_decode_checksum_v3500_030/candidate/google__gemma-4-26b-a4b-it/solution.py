def run(ids, dict_vals, iters: int) -> int: 
    if not ids:
        # If no ids, the loop runs, h is reset, and the loop over decoded is empty.
        # The last value of h will be the offset basis.
        return 1469598103934665603

    # The decoded values are constant across all iterations because ids and dict_vals don't change.
    # The loop 'for _ in range(iters)' resets 'h' to the offset basis at the start of every iteration.
    # Therefore, the result is simply the hash of the decoded values calculated once.
    
    # Pre-calculate decoded values
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in decoded:
        h ^= (v + 1)
        h = (h * 1099511628211) & mask
    
    return h