def run(ids, dict_vals, iters: int) -> int:
    # Match baseline behavior: if no iterations, return initial h (0)
    if iters <= 0:
        return 0

    MASK = (1 << 64) - 1
    PRIME = 1099511628211
    h = 1469598103934665603

    d_get = dict_vals.__getitem__

    for idx in ids:
        v = d_get(idx)
        h = ((h ^ (v + 1)) * PRIME) & MASK

    return h
