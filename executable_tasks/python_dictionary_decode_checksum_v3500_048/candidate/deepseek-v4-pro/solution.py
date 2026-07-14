def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    dv = dict_vals
    for idx in ids:
        v = dv[idx]
        h ^= (v + 1)
        h = (h * mult) & mask
    return h
