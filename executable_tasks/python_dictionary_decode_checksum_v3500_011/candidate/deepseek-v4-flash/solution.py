def run(ids, dict_vals, iters: int) -> int:
    vals = [dict_vals[idx] for idx in ids]
    h = 0
    offset = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1
    for _ in range(iters):
        h = offset
        for v in vals:
            h ^= v + 1
            h = (h * prime) & mask
    return h