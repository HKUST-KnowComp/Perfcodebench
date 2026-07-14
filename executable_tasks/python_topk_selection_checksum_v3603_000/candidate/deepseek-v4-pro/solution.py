def run(scores: list[int], k: int, iters: int) -> int:
    import heapq
    h = 0
    for _ in range(iters):
        top_k = heapq.nsmallest(k, enumerate(scores), key=lambda x: (-x[1], x[0]))
        h = 1469598103934665603
        for index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
