import numpy as np

def run(scores: list[int], k: int, iters: int) -> int:
    arr = np.asarray(scores, dtype=np.int64)
    n = len(arr)
    if k <= 0 or n == 0:
        h = 0
        for _ in range(iters):
            h = 1469598103934665603
        return h

    # Stable argsort descending: negate scores, stable sort gives ties by ascending index
    neg = -arr
    order = np.argsort(neg, kind='stable')[:k]
    top_scores = arr[order]
    top_indices = order.astype(np.int64)

    # Precompute combined values for checksum
    combined = (top_scores << 32) ^ top_indices

    # FNV-style hash constants
    MOD = (1 << 64)
    MULT = np.uint64(1099511628211)

    h = 0
    for _ in range(iters):
        h_val = np.uint64(1469598103934665603)
        for c in combined:
            h_val ^= np.uint64(c)
            h_val *= MULT
        h = int(h_val)
    return h
