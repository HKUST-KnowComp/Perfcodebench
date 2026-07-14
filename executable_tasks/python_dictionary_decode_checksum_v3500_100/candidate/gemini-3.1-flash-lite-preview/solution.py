def run(ids, dict_vals, iters: int) -> int:
    h = 0
    # Pre-fetch dict_vals for faster access
    # Using a local variable for the constant to avoid repeated attribute/global lookups
    fnv_prime = 1099511628211
    fnv_offset = 1469598103934665603
    mask = (1 << 64) - 1

    for _ in range(iters):
        h = fnv_offset
        for idx in ids:
            # Inline the FNV-1a logic directly on the lookup
            h = ((h ^ (dict_vals[idx] + 1)) * fnv_prime) & mask
    return h