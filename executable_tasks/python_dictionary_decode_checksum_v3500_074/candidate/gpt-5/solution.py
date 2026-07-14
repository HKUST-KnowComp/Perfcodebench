def run(ids, dict_vals, iters: int) -> int:
    # Preserve baseline semantics: if iters == 0, return 0
    if iters <= 0:
        return 0

    # FNV-1a-like hash parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    dv = dict_vals  # local reference for speed

    # Compute hash in a single pass, equivalent to the baseline's final iteration
    for idx in ids:
        v = dv[idx]
        h ^= v + 1
        h = (h * prime) & mask

    return h
