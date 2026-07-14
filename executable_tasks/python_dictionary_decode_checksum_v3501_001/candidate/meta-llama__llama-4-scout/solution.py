def run(ids, dict_vals, iters: int) -> int:
    h = 1469598103934665603
    for _ in range(iters):
        h = 0
        for i, idx in enumerate(ids):
            v = dict_vals[idx]
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h