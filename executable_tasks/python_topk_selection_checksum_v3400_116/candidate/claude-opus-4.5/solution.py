import numpy as np

def run(scores: list[int], k: int, iters: int) -> int:
    arr = np.asarray(scores, dtype=np.int64)
    # argsort with stable sort: -arr gives descending scores, stable preserves index order for ties
    order = np.argsort(-arr, kind='stable')
    top_indices = order[:k]
    top_scores = arr[top_indices]
    
    # Precompute combined values for hash
    combined = (top_scores << 32) ^ top_indices
    
    mask = np.uint64((1 << 64) - 1)
    mult = np.uint64(1099511628211)
    init = np.uint64(1469598103934665603)
    
    h = np.uint64(0)
    for _ in range(iters):
        h = init
        for c in combined:
            h ^= np.uint64(c)
            h = (h * mult) & mask
    return int(h)
