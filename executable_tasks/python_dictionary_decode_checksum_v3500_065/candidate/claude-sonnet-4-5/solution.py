def run(ids, dict_vals, iters: int) -> int:
    h = 0
    const_mult = 1099511628211
    mask = (1 << 64) - 1
    for _ in range(iters):
        h = 1469598103934665603
        for idx in ids:
            h ^= dict_vals[idx] + 1
            h = (h * const_mult) & mask
    return h
