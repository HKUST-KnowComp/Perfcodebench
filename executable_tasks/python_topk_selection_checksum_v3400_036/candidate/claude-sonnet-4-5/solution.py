import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    pairs = [(-s, i, s) for i, s in enumerate(scores)]
    h = 0
    for _ in range(iters):
        top_k = heapq.nsmallest(k, pairs)
        h = 1469598103934665603
        for _, index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h