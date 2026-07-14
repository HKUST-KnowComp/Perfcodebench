def run(ids, dict_vals, iters):
    if iters == 0:
        return 0
    vals = [dict_vals[idx] + 1 for idx in ids]
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for v in vals:
        h ^= v
        h = (h * 1099511628211) & mask
    return h