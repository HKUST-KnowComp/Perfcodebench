def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    dvp1 = [v + 1 for v in dict_vals]
    h = 1469598103934665603
    MUL = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF
    for idx in ids:
        h = ((h ^ dvp1[idx]) * MUL) & MASK
    return h