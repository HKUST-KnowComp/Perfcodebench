def run(ids, dict_vals, iters: int) -> int:
    decoded = [0] * len(ids)
    h = 0
    for _ in range(iters):
        for i, idx in enumerate(ids):
            decoded[i] = dict_vals[idx]
        h = 1469598103934665603
        for v in decoded:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
