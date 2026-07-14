def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    mul = 1099511628211
    get = dict_vals.__getitem__
    for idx in ids:
        v = get(idx)
        h ^= v + 1
        h = (h * mul) & mask
    return h