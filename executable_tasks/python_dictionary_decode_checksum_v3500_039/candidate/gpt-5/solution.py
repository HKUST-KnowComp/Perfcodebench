def run(ids, dict_vals, iters: int) -> int:
    # If no iterations are requested, mirror baseline behavior: return 0.
    if iters <= 0:
        return 0

    # Constants for 64-bit FNV-1a-like hashing used in baseline
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    dv = dict_vals  # local reference for faster access

    # Directly hash the decoded values in a single pass.
    for idx in ids:
        h ^= dv[idx] + 1
        h = (h * prime) & mask

    return h
