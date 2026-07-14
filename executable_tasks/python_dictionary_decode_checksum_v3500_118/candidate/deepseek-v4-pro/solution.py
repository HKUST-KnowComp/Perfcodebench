def run(ids, dict_vals, iters: int) -> int:
    mask = (1 << 64) - 1
    mult = 1099511628211
    h = 1469598103934665603
    for idx in ids:
        v = dict_vals[idx]
        h ^= v + 1
        h = (h * mult) & mask
    return h
