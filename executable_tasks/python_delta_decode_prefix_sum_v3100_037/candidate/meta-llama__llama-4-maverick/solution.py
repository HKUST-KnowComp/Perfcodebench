```python
import numpy as np

def run(deltas: list[int], iters: int) -> int:
    deltas = np.array(deltas, dtype=np.int64)
    total = 0
    prefix_sum = np.zeros(len(deltas) + 1, dtype=np.int64)
    for _ in range(iters):
        np.cumsum(deltas, out=prefix_sum[1:])
        total = (prefix_sum[:-1] + 0x9E3779B1).sum()
    return total
```