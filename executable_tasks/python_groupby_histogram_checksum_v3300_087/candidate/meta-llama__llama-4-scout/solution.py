def run(keys, vals, buckets: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        hist_dict = {}
        for k, v in zip(keys, vals):
            if k in hist_dict:
                hist_dict[k] += v
            else:
                hist_dict[k] = v
        hist = [hist_dict.get(i, 0) for i in range(buckets)]
        h = 1469598103934665603
        for v in hist:
            h ^= v + 1
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h