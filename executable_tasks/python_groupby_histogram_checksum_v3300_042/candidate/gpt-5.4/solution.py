def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    n = len(keys)
    use_range = hasattr(keys, '__getitem__') and hasattr(vals, '__getitem__')

    h = 0
    for _ in range(iters):
        hist = [0] * buckets

        if use_range:
            for i in range(n):
                hist[keys[i]] += vals[i]
        else:
            for k, v in zip(keys, vals):
                hist[k] += v

        h = offset
        for v in hist:
            h = ((h ^ (v + 1)) * prime) & mask

    return h
