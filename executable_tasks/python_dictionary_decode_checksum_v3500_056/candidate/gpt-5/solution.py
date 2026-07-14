def run(ids, dict_vals, iters: int) -> int:
    # Compute the checksum directly from the decoded values once.
    # The original code reinitialized the hash each iteration and processed
    # the same decoded values, so the final checksum is independent of iters.
    mask = 0xFFFFFFFFFFFFFFFF
    h = 1469598103934665603
    p = 1099511628211
    dv = dict_vals

    for idx in ids:
        v = dv[idx]
        h ^= v + 1
        h = (h * p) & mask

    return h
