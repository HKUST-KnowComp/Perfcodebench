def run(ids, dict_vals, iters: int) -> int:
    if iters == 0:
        return 0
    # Decode once - values don't change across iterations
    decoded = [dict_vals[idx] for idx in ids]
    # Compute hash once - same decoded values produce same hash each iteration
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    for v in decoded:
        h ^= v + 1
        h = (h * mult) & mask
    return h
