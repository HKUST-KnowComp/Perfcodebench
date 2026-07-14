def run(ids, dict_vals, iters: int) -> int:
    if iters <= 0:
        return 0
    h = 1469598103934665603
    mul = 1099511628211
    mask = (1 << 64) - 1
    for idx in ids:
        v = dict_vals[idx]
        h ^= (v + 1)
        h = (h * mul) & mask
    return h
