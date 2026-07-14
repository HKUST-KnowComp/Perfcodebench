import numpy as np

def run(scores: list[int], k: int, iters: int) -> int:
    arr = np.asarray(scores, dtype=np.int64)
    # argsort with stable sort: -arr gives descending scores, stable preserves index order for ties
    order = np.argsort(-arr, kind='stable')
    top_idx = order[:k]
    top_scores = arr[top_idx]
    
    mask = np.uint64((1 << 64) - 1)
    mult = np.uint64(1099511628211)
    
    h = np.uint64(0)
    for _ in range(iters):
        h = np.uint64(1469598103934665603)
        for j in range(k):
            sc = np.uint64(top_scores[j])
            idx = np.uint64(top_idx[j])
            h ^= (sc << np.uint64(32)) ^ idx
            h = (h * mult) & mask
    return int(h)
