def run(ids, dict_vals, iters: int) -> int:
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for idx in ids:
        v = dict_vals[idx]
        h ^= v + 1
        h = (h * prime) & mask
    return h