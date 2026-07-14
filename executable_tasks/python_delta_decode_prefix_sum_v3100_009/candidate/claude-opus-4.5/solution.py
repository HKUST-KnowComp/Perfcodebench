import numpy as np

def run(deltas: list[int], iters: int) -> int:
    arr = np.array(deltas, dtype=np.int64)
    n = len(deltas)
    magic = np.int64(0x9E3779B1)
    for _ in range(iters):
        values = np.cumsum(arr)
        total = np.sum(values) + n * magic
    return int(total)
