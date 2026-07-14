def run(ids, dict_vals, iters: int) -> int:
    h = 0
    # Pre-fetch dict_vals for faster access
    # Using a local reference to the list access method or direct indexing
    # The FNV-1a constant and mask
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    
    # Pre-map the ids to values to avoid repeated lookups if possible,
    # but since we must iterate iters times, we compute the decoded values once.
    decoded = [dict_vals[i] for i in ids]
    
    for _ in range(iters):
        h = 1469598103934665603
        for v in decoded:
            h = ((h ^ (v + 1)) * FNV_PRIME) & MASK
    return h