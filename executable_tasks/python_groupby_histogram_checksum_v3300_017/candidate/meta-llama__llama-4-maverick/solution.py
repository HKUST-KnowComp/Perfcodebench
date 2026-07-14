import numpy as np

def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    keys = np.array(keys, dtype=np.int32)
    vals = np.array(vals, dtype=np.int32)
    for _ in range(iters):
        hist = np.bincount(keys, weights=vals, minlength=buckets)
        h = 1469598103934665603
        h ^= np.bitwise_xor.reduce(hist + 1)  # XOR with the sum of (hist + 1)
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h