def run(ids, dict_vals, iters: int) -> int:
    # If no iterations, baseline returns 0 since the loop body never executes.
    if iters <= 0:
        return 0

    # FNV-like 64-bit hash parameters
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1

    # Bind method locally to reduce attribute lookup overhead
    dv_get = dict_vals.__getitem__

    # Stream decoding and hashing in a single pass, avoiding intermediate list creation.
    for idx in ids:
        v = dv_get(idx)
        h ^= (v + 1)
        h = (h * mult) & mask

    return h
