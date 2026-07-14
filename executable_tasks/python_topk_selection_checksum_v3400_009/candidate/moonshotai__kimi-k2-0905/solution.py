import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # pre-compute the key function to avoid lambda overhead inside the loop
    keyed = [(-s, i) for i, s in enumerate(scores)]
    for _ in range(iters):
        # O(n + k log k) instead of O(n log n)
        topk = heapq.nlargest(k, keyed)
        # sort the small top-k slice to ensure stable ordering by index
        topk.sort()
        h = 1469598103934665603
        for neg_score, index in topk:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h