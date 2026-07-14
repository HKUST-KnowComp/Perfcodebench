def run(ids, dict_vals, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603
    dvals = dict_vals
    ids_local = ids

    h = offset
    for _ in range(iters):
        h = offset
        for idx in ids_local:
            h ^= dvals[idx] + 1
            h = (h * prime) & mask
    return h
