def run(ids, dict_vals, iters: int) -> int:
    OFFSET = 1469598103934665603
    PRIME = 1099511628211
    MASK = (1 << 64) - 1
    if iters <= 0:
        return 0
    ids_local = ids
    dict_local = dict_vals
    h = 0
    for _ in range(iters):
        h = OFFSET
        for idx in ids_local:
            v = dict_local[idx] + 1
            h = (h ^ v) * PRIME & MASK
    return h