def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    fnv_init = 1469598103934665603
    fnv_mul = 1099511628211
    h = 0
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = fnv_init
        for v in hist:
            h = ((h ^ (v + 1)) * fnv_mul) & mask
    return h