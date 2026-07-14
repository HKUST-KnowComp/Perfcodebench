import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-compute the initial FNV-1a offset basis once
    base = 1469598103934665603
    for _ in range(iters):
        # O(n log k) instead of O(n log n)
        topk = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)))
        h = base
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h