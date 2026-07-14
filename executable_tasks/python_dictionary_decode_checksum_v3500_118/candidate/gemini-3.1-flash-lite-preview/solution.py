def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals to local variable for faster access
    # The FNV-1a constant and mask
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    h = FNV_OFFSET
    # Optimization: Avoid list allocation inside the loop.
    # Perform the hash calculation directly on the values retrieved from dict_vals.
    for _ in range(iters):
        h = FNV_OFFSET
        for idx in ids:
            h ^= (dict_vals[idx] + 1)
            h = (h * FNV_PRIME) & MASK
    return h