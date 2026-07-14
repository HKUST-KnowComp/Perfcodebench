import numpy as np
import numba

@numba.njit(cache=True)
def _compute_hash(scores_arr, indices_arr):
    h = np.uint64(1469598103934665603)
    mult = np.uint64(1099511628211)
    mask = np.uint64((1 << 64) - 1)
    for i in range(len(scores_arr)):
        s = np.uint64(scores_arr[i])
        idx = np.uint64(indices_arr[i])
        h ^= (s << np.uint64(32)) ^ idx
        h = (h * mult) & mask
    return int(h)

def run(scores: list[int], k: int, iters: int) -> int:
    arr = np.asarray(scores, dtype=np.int64)
    n = len(arr)
    if k <= 0:
        h = 0
        for _ in range(iters):
            h = 1469598103934665603
        return h
    if k >= n:
        # All elements, sort by (-score, index)
        indices = np.arange(n, dtype=np.int64)
        order = np.lexsort((indices, -arr))
        top_scores = arr[order]
        top_indices = order.astype(np.int64)
        h = 0
        for _ in range(iters):
            h = _compute_hash(top_scores, top_indices)
        return h
    # Partial selection: find top-k by score (descending), then by index (ascending)
    # argpartition on -arr gives indices of k largest scores
    neg_arr = -arr
    part_idx = np.argpartition(neg_arr, k - 1)[:k]
    # Now sort these k by (-score, index)
    top_scores_unsorted = arr[part_idx]
    order_in_top = np.lexsort((part_idx, -top_scores_unsorted))
    top_indices = part_idx[order_in_top].astype(np.int64)
    top_scores = top_scores_unsorted[order_in_top]
    h = 0
    for _ in range(iters):
        h = _compute_hash(top_scores, top_indices)
    return h
