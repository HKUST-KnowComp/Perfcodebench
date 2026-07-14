def run(ids, dict_vals, iters: int) -> int:
    # Precompute all values once, avoid redundant work across iterations
    pre_vals = [dict_vals[idx] + 1 for idx in ids]
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = FNV_OFFSET
        for val in pre_vals:
            h ^= val
            h = (h * FNV_PRIME) & MASK
    return h