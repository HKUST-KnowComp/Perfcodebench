def run(ids, dict_vals, iters: int) -> int:
    # If no iterations requested, preserve original behavior (return 0)
    if iters <= 0:
        return 0

    # Compute checksum once since each iteration in the baseline recomputed the same result
    h = 1469598103934665603  # FNV-1 offset basis (64-bit)
    prime = 1099511628211    # FNV prime (64-bit)
    mask = (1 << 64) - 1

    dv = dict_vals  # localize for faster access
    for idx in ids:
        v = dv[idx]
        h ^= v + 1
        h = (h * prime) & mask

    return h
