def run(ids, dict_vals, iters: int) -> int:
    init_h = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    get_value = dict_vals.__getitem__
    h = 0
    for _ in range(iters):
        h = init_h
        for v in map(get_value, ids):
            h ^= v + 1
            h = (h * multiplier) & mask
    return h
