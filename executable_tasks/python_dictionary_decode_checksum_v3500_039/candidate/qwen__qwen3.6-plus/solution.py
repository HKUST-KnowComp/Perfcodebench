def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    h = 1469598103934665603
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    for idx in ids:
        h ^= dict_vals[idx] + 1
        h = (h * prime) & mask
    return h