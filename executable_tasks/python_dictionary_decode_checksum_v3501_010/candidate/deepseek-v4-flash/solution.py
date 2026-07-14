def run(ids, dict_vals, iters: int) -> int:
    # Precompute decoded values once (dict_vals is constant per call)
    decoded = [dict_vals[idx] for idx in ids]
    h = 0
    INIT = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    for _ in range(iters):
        h = INIT
        for v in decoded:
            h ^= v + 1
            h = (h * MULT) & MASK
    return h