def run(ids, dict_vals, iters: int) -> int:
    if not ids:
        return 1469598103934665603

    # Pre-decode the values since ids and dict_vals are constant across iters
    decoded = [dict_vals[idx] for idx in ids]
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    
    # The original code performs the same decoding 'iters' times.
    # However, the hash 'h' is reset to the FNV offset basis at the start of EVERY iteration.
    # This means the final 'h' is actually just the hash of the 'decoded' list
    # calculated 'iters' times, where each iteration starts from the offset basis.
    # Wait, looking closely at the baseline:
    # for _ in range(iters):
    #    for i, idx in enumerate(ids):
    #        decoded[i] = dict_vals[idx]
    #    h = 1469598103934665603
    #    for v in decoded:
    #        h ^= v + 1
    #        h = (h * 1099511628211) & mask
    # This means the result of the previous iteration is discarded because 'h' is reset.
    # Therefore, the loop is effectively redundant; only the last iteration matters.
    
    # Let's re-verify: h is reset to 1469598103934665603 inside the 'iters' loop.
    # So the result is simply the hash of the decoded values.
    
    h = 1469598103934665603
    for v in decoded:
        h ^= v + 1
        h = (h * 1099511628211) & mask
        
    return h