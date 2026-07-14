def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    MASK = 0xFFFFFFFFFFFFFFFF
    MULT = 1099511628211
    h = 1469598103934665603
    d = dict_vals
    for idx in ids:
        v = d[idx]
        h ^= v + 1
        h = (h * MULT) & MASK
    return h
