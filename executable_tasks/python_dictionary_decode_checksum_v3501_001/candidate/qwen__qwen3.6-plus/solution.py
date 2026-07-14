def run(ids, dict_vals, iters: int) -> int:
    if iters <= 0:
        return 0
    decoded = [dict_vals[i] + 1 for i in ids]
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for _ in range(iters):
        for v in decoded:
            h = ((h ^ v) * prime) & mask
    return h