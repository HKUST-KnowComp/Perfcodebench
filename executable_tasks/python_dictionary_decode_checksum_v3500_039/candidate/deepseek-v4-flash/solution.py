def run(ids, dict_vals, iters: int) -> int:
    mask = (1 << 64) - 1
    base = 1469598103934665603
    prime = 1099511628211
    h = 0
    for _ in range(iters):
        h = base
        for idx in ids:
            v = dict_vals[idx]
            h ^= v + 1
            h = (h * prime) & mask
    return h