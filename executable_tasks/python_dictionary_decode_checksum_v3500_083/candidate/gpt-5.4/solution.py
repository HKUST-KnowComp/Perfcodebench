def run(ids, dict_vals, iters: int) -> int:
    # Cache frequently used objects/values in locals for faster access.
    ids_local = ids
    dict_local = dict_vals
    n = len(ids_local)
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603

    # Precompute decoded values once since ids and dict_vals do not change across iterations.
    decoded = [0] * n
    for i in range(n):
        decoded[i] = dict_local[ids_local[i]]

    h = 0
    for _ in range(iters):
        h = offset
        for v in decoded:
            h = ((h ^ (v + 1)) * prime) & mask
    return h
