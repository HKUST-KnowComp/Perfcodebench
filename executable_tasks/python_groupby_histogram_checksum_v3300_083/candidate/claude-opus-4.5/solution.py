def run(keys, vals, buckets: int, iters: int) -> int:
    import numpy as np
    # Compute histogram once using fast C-level bincount
    k = np.asarray(keys)
    v = np.asarray(vals)
    hist_arr = np.bincount(k, weights=v, minlength=buckets).astype(np.int64)
    hist = hist_arr.tolist()
    
    # Run checksum computation for each iteration
    h = 0
    mask = (1 << 64) - 1
    mult = 1099511628211
    init = 1469598103934665603
    for _ in range(iters):
        h = init
        for val in hist:
            h ^= val + 1
            h = (h * mult) & mask
    return h
