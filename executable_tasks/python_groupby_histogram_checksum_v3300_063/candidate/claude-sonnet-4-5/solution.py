def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    keys_list = keys if isinstance(keys, list) else list(keys)
    vals_list = vals if isinstance(vals, list) else list(vals)
    n = len(keys_list)
    
    for _ in range(iters):
        hist = [0] * buckets
        for i in range(n):
            hist[keys_list[i]] += vals_list[i]
        
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h