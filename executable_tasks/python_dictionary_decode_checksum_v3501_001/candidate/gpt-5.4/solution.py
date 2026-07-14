def run(ids, dict_vals, iters: int) -> int:
    n = len(ids)
    if n == 0:
        return 1469598103934665603

    mask = 0xFFFFFFFFFFFFFFFF
    mul = 1099511628211
    h0 = 1469598103934665603

    decoded = [0] * n
    ids_local = ids
    dict_local = dict_vals

    for _ in range(iters):
        for i in range(n):
            decoded[i] = dict_local[ids_local[i]]

        h = h0
        for i in range(n):
            h = ((h ^ (decoded[i] + 1)) * mul) & mask

    return h
