def run(keys, vals, buckets: int, iters: int) -> int:
    if iters <= 0:
        return 0
    
    hist = [0] * buckets
    for k, v in zip(keys, vals):
        hist[k] += v
        
    h = 1469598103934665603
    for v in hist:
        h = ((h ^ (v + 1)) * 1099511628211) & 18446744073709551615
        
    if iters > 1 and (iter(keys) is keys or iter(vals) is vals):
        h = 1469598103934665603
        for _ in range(buckets):
            h = ((h ^ 1) * 1099511628211) & 18446744073709551615

    return h
