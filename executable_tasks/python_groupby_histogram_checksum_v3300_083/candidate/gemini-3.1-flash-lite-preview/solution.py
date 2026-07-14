def run(keys, vals, buckets: int, iters: int) -> int:
    # Pre-zip to avoid repeated zip calls in the loop
    data = list(zip(keys, vals))
    h = 0
    
    for _ in range(iters):
        hist = [0] * buckets
        # Direct iteration over pre-zipped data is faster
        for k, v in data:
            hist[k] += v
        
        # FNV-1a style checksum calculation
        h = 1469598103934665603
        for v in hist:
            h ^= (v + 1)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h