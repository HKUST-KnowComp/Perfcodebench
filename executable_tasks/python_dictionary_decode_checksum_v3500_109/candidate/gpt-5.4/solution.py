def run(ids, dict_vals, iters: int) -> int:
    n = len(ids)
    if n == 0:
        return 1469598103934665603

    # Precompute the decoded values once since ids and dict_vals do not change
    # across iterations. The baseline redundantly decodes on every iteration.
    decoded = [0] * n
    for i in range(n):
        decoded[i] = dict_vals[ids[i]]

    mul = 1099511628211
    mask = (1 << 64) - 1
    base = 1469598103934665603

    h = base
    for _ in range(iters):
        h = base
        for v in decoded:
            h = ((h ^ (v + 1)) * mul) & mask
    return h
