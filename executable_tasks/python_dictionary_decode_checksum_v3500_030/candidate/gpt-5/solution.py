FNV_OFFSET_64 = 1469598103934665603
FNV_PRIME_64 = 1099511628211
MASK_64 = (1 << 64) - 1


def run(ids, dict_vals, iters: int) -> int:
    # If no iterations, baseline returns initial h = 0
    if iters <= 0:
        return 0

    # Compute the checksum once since each iteration yields the same result
    h = FNV_OFFSET_64
    dv = dict_vals  # local binding for speed
    prime = FNV_PRIME_64
    mask = MASK_64

    for idx in ids:
        v = dv[idx]
        h ^= (v + 1)
        h = (h * prime) & mask

    return h
