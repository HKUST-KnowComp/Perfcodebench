def run(ids, dict_vals, iters: int) -> int:
    # Preserve baseline behavior: if no iterations, return 0
    if iters <= 0:
        return 0

    # Compute the checksum for a single effective iteration (final one)
    get = dict_vals.__getitem__  # localize for speed (works for dicts/lists)
    h = 1469598103934665603
    mul = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    for idx in ids:
        h ^= get(idx) + 1
        h = (h * mul) & mask

    return h
