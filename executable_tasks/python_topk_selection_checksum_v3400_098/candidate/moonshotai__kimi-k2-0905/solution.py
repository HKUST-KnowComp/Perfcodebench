import sys

sys.setrecursionlimit(10000)

def _partition_topk(arr, low, high, k):
    """Return the k largest items in arr (inclusive of k) using quick-select."""
    left = low
    right = high
    pivot_idx = low + (high - low) // 2
    pivot_val = arr[pivot_idx]
    arr[pivot_idx], arr[right] = arr[right], arr[pivot_idx]
    store = left
    for i in range(left, right):
        if arr[i][0] > pivot_val[0]:
            arr[store], arr[i] = arr[i], arr[store]
            store += 1
    arr[right], arr[store] = arr[store], arr[right]
    if store < k:
        _partition_topk(arr, store + 1, high, k)
    elif store > k:
        _partition_topk(arr, left, store - 1, k)
    return arr

def run(scores, k, iters):
    """Fast top-k selection returning the same checksum result as the baseline."""
    h = 0
    n = len(scores)
    if k <= 0 or k > n:
        k = n
    for _ in range(iters):
        data = [(score, idx) for idx, score in enumerate(scores)]
        _partition_topk(data, 0, n - 1, k)
        topk = sorted(data[:k], key=lambda x: (-x[0], x[1]))
        h = 1469598103934665603
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h