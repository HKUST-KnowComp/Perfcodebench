def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals to local variable for faster access
    # Pre-calculate the decoded values once since they are constant across iters
    decoded = [dict_vals[i] for i in ids]
    
    h = 0
    # FNV-1a constants
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    for _ in range(iters):
        h = FNV_OFFSET_BASIS
        for v in decoded:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
            
    return h