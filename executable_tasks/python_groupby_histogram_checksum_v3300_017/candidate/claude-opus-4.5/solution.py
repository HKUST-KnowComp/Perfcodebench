def run(keys, vals, buckets: int, iters: int) -> int:
    # Precompute histogram once using dict for fast aggregation
    hist_dict = {}
    for k, v in zip(keys, vals):
        if k in hist_dict:
            hist_dict[k] += v
        else:
            hist_dict[k] = v
    
    # Convert to list for checksum computation
    hist = [0] * buckets
    for k, v in hist_dict.items():
        hist[k] = v
    
    # Run checksum computation iters times
    h = 0
    mask = (1 << 64) - 1
    mult = 1099511628211
    for _ in range(iters):
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * mult) & mask
    return h
