def run(ids, dict_vals, iters: int) -> int:
    # Pre-fetch dict_vals as a local list for faster access
    # The checksum logic is FNV-1a variant
    h = 0
    # Pre-calculate the mapping to avoid repeated lookups if ids are reused
    mapped = [dict_vals[i] for i in ids]
    
    for _ in range(iters):
        h = 1469598103934665603
        for v in mapped:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h