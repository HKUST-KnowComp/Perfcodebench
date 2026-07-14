def run(keys, vals, buckets: int, iters: int) -> int:
    # If no iterations requested, mirror baseline behavior
    if iters <= 0:
        return 0

    # Build histogram once (same for every iteration in baseline)
    hist = [0] * buckets

    k_list = keys
    v_list = vals
    n = len(k_list)
    h_local = hist  # local alias for speed

    # Accumulate values into histogram buckets
    for i in range(n):
        h_local[k_list[i]] += v_list[i]

    # FNV-1a 64-bit checksum over buckets
    h = 1469598103934665603
    prime = 1099511628211
    mask = (1 << 64) - 1

    for val in h_local:
        h ^= (val + 1)
        h = (h * prime) & mask

    return h
