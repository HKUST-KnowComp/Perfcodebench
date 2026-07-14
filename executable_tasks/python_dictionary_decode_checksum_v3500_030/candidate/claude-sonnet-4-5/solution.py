def run(ids, dict_vals, iters: int) -> int:
    h = 0
    mask = (1 << 64) - 1
    mult = 1099511628211
    for _ in range(iters):
        h = 1469598103934665603
        for idx in ids:
            h ^= dict_vals[idx] + 1
            h = (h * mult) & mask
    return h
