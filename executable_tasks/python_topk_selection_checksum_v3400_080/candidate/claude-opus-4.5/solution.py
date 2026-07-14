import numpy as np

def run(scores: list[int], k: int, iters: int) -> int:
    arr = np.asarray(scores, dtype=np.int64)
    n = len(arr)
    if k <= 0 or n == 0:
        h = 0
        for _ in range(iters):
            h = 1469598103934665603
        return h

    # Stable argsort on negated scores gives descending order with ties broken by index
    neg = -arr
    order = np.argsort(neg, kind='stable')
    top_idx = order[:k]
    top_scores = arr[top_idx]

    # Precompute combined values for checksum
    combined = (top_scores.astype(np.uint64) << 32) ^ top_idx.astype(np.uint64)

    mask = np.uint64((1 << 64) - 1)
    mult = np.uint64(1099511628211)
    init = np.uint64(1469598103934665603)

    h = np.uint64(0)
    for _ in range(iters):
        h = init
        for c in combined:
            h ^= c
            h = (h * mult) & mask

    return int(h)
