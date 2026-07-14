import numpy as np

def run(deltas: list[int], iters: int) -> int:
    if iters == 0:
        return 0
    arr = np.array(deltas, dtype=np.int64)
    prefix = np.cumsum(arr)
    n = len(deltas)
    total = int(np.sum(prefix)) + n * 0x9E3779B1
    return total
