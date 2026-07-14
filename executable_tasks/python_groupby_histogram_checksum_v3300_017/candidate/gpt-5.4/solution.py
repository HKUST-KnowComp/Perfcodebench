def run(keys, vals, buckets: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    offset = 1469598103934665603
    prime = 1099511628211

    keys_local = keys
    vals_local = vals
    buckets_local = buckets
    zip_local = zip

    h = 0
    for _ in range(iters):
        hist = [0] * buckets_local
        for k, v in zip_local(keys_local, vals_local):
            hist[k] += v

        hh = offset
        for v in hist:
            hh = ((hh ^ (v + 1)) * prime) & mask
        h = hh
    return h
