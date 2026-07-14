def run(ids, dict_vals, iters: int) -> int:
    n = len(ids)
    if n == 0:
        return 1469598103934665603

    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603

    ids_local = ids
    dict_local = dict_vals

    h = offset
    for _ in range(iters):
        h = offset
        for i in range(n):
            h ^= dict_local[ids_local[i]] + 1
            h = (h * prime) & mask
    return h
