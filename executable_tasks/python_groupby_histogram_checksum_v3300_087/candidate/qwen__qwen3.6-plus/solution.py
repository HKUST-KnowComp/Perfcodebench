_MASK = 0xFFFFFFFFFFFFFFFF
_PRIME = 1099511628211
_OFFSET = 1469598103934665603

def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = _OFFSET
        for v in hist:
            h = ((h ^ (v + 1)) * _PRIME) & _MASK
    return h