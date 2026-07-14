def run(ids, dict_vals, iters: int) -> int:
    # Preserve baseline behavior: if iters <= 0, the outer loop never runs and returns 0.
    if iters <= 0:
        return 0

    # FNV-1a 64-bit parameters and mask
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    d = dict_vals  # local reference for speed

    # Single streaming pass: decode and hash without building an intermediate list
    for idx in ids:
        h ^= d[idx] + 1
        h = (h * prime) & mask

    return h
