def run(ids, dict_vals, iters: int) -> int:
    # Bind frequently used objects/constants to locals for faster access.
    ids_local = ids
    dict_local = dict_vals
    n = len(ids_local)
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    h0 = 1469598103934665603

    # Fast path for empty input.
    if n == 0:
        return h0

    # Decode and hash in a single pass to avoid allocating/writing an
    # intermediate decoded list each iteration.
    h = h0
    for _ in range(iters):
        h = h0
        for idx in ids_local:
            h ^= dict_local[idx] + 1
            h = (h * prime) & mask
    return h
