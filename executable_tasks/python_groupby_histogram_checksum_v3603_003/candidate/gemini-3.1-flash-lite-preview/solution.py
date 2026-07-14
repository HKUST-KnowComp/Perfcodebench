def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    # Pre-bind local variables for faster access in the loop
    for _ in range(iters):
        hist = [0] * buckets
        # Using a simple loop is often faster than zip in tight Python loops
        # if the overhead of zip creation is significant, but here we optimize
        # the aggregation step.
        for i in range(len(keys)):
            hist[keys[i]] += vals[i]
        
        h = 1469598103934665603
        # FNV-1a hash calculation
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h