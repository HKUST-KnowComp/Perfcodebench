def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    h = 1469598103934665603
    p = 1099511628211
    m = 0xFFFFFFFFFFFFFFFF
    for idx in ids:
        h ^= dict_vals[idx] + 1
        h = (h * p) & m
    return h