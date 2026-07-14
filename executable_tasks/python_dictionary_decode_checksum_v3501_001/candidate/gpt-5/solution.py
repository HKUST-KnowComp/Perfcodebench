def run(ids, dict_vals, iters: int) -> int:
    # Preserve baseline behavior: if iters == 0, no hashing occurs and 0 is returned.
    if iters <= 0:
        return 0

    # Compute the checksum exactly once, since the final result after any positive
    # number of iterations is identical to a single iteration in the baseline.
    h = 1469598103934665603  # FNV-1a 64-bit offset basis
    d = dict_vals
    mul = 1099511628211      # FNV-1a 64-bit prime
    mask = 0xFFFFFFFFFFFFFFFF

    for idx in ids:
        h ^= d[idx] + 1
        h = (h * mul) & mask

    return h
