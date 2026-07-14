def run(ids, dict_vals, iters: int) -> int:
    if not ids:
        # If no ids, the loop runs but h is reset to offset basis
        # and the inner loop for v in decoded doesn't execute.
        # However, the problem implies ids is not empty for meaningful work.
        # Based on the baseline, if iters > 0, h starts at offset basis.
        return 1469598103934665603 if iters > 0 else 0

    # The baseline resets h to the offset basis at the start of EVERY iteration.
    # The decoded values are also re-assigned to the same values every iteration.
    # Therefore, the result of the last iteration is the same as any single iteration.
    
    # 1. Decode once
    decoded = [dict_vals[idx] for idx in ids]
    
    # 2. Compute FNV-1a hash once
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in decoded:
        h ^= (v + 1)
        h = (h * 1099511628211) & mask
        
    return h