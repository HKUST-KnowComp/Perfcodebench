import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    keys = np.asarray(keys, dtype=np.intp)
    vals = np.asarray(vals, dtype=np.int64)
    hist = np.zeros(buckets, dtype=np.int64)
    
    for _ in range(iters):
        hist.fill(0)
        np.add.at(hist, keys, vals)
        
        h = np.uint64(1469598103934665603)
        for v in hist:
            h ^= np.uint64(v + 1)
            h = (h * np.uint64(1099511628211)) & np.uint64(0xFFFFFFFFFFFFFFFF)
    return int(h)