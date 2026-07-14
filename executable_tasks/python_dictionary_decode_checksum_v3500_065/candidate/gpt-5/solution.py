def run(ids, dict_vals, iters: int) -> int:
    # Preserve baseline behavior for non-positive iterations
    if iters <= 0:
        return 0

    dv = dict_vals
    h = 1469598103934665603  # FNV offset basis
    mul = 1099511628211      # FNV prime
    mask = (1 << 64) - 1

    # Fuse decode and hash into a single pass
    for idx in ids:
        h ^= dv[idx] + 1
        h = (h * mul) & mask

    return h
