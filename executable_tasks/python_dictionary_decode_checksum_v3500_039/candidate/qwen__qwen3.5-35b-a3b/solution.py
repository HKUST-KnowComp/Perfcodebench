def run(ids, dict_vals, iters: int) -> int:
    decoded = [dict_vals[idx] for idx in ids]
    h = 1469598103934665603
    for v in decoded:
        h ^= v + 1
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h