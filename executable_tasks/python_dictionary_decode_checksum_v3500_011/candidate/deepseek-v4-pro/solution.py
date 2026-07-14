def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    h = 1469598103934665603
    MULT = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    d = dict_vals
    for idx in ids:
        h ^= d[idx] + 1
        h = (h * MULT) & MASK
    return h
