def run(keys, vals, buckets: int, iters: int) -> int:
    # Pre-bind local variables for faster access in the loop
    h_final = 0
    
    for _ in range(iters):
        hist = [0] * buckets
        # Using zip is standard, but localizing the list access is key
        for k, v in zip(keys, vals):
            hist[k] += v
        
        # FNV-1a hash implementation
        h = 1469598103934665603
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        h_final = h
        
    return h_final