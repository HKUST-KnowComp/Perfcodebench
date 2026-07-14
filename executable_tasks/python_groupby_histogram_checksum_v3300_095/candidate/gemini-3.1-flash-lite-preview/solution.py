def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-bind local variables for faster access in the loop
    for _ in range(iters):
        hist = [0] * buckets
        # Using zip is standard, but iterating by index can be slower in Python.
        # The current approach is efficient for standard Python lists.
        for k, v in zip(keys, vals):
            hist[k] += v
        
        h = 1469598103934665603
        # Unrolling or using a generator expression can be slower than a simple loop
        # for FNV-1a due to the bitwise operations and modulo requirements.
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h