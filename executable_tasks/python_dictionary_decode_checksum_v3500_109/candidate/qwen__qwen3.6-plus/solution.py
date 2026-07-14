def run(ids, dict_vals, iters: int) -> int:
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    FNV_OFFSET = 1469598103934665603
    h = 0
    for _ in range(iters):
        h = FNV_OFFSET
        for idx in ids:
            h = ((h ^ (dict_vals[idx] + 1)) * FNV_PRIME) & MASK
    return h