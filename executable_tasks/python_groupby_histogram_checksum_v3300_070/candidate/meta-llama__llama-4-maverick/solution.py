```python
def run(keys, vals, buckets: int, iters: int) -> int:
    hash_const = 1469598103934665603
    hash_mult = 1099511628211
    mask = (1 << 64) - 1
    h = hash_const
    for _ in range(iters):
        hist = [0] * buckets
        for k, v in zip(keys, vals):
            hist[k] += v
        h = hash_const
        for v in hist:
            h = (h ^ (v + 1)) * hash_mult & mask
    return h
```