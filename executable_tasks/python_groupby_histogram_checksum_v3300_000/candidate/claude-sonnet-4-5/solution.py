from array import array

def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    pairs = list(zip(keys, vals))
    for _ in range(iters):
        hist = array('q', [0] * buckets)
        for k, v in pairs:
            hist[k] += v
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
