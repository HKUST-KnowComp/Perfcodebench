def run(ids, dict_vals, iters: int) -> int:
    decoded = [dict_vals[idx] for idx in ids]
    h_init = 1469598103934665603
    multiplier = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF
    for _ in range(iters):
        h = h_init
        for v in decoded:
            h ^= v + 1
            h = (h * multiplier) & mask
    return h