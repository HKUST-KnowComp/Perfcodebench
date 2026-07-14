```python
import numpy as np

def run(ids, dict_vals, iters: int) -> int:
    ids_array = np.array(ids, dtype=np.int64)
    dict_vals_array = np.array(dict_vals, dtype=np.int64)
    decoded = np.take(dict_vals_array, ids_array)
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        h = (np.int64(0xFFFFFFFFFFFFFFFF) & (h + np.dot(decoded + 1, np.uint64(1099511628211))))
        for _ in range(len(ids) - 1):
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
```