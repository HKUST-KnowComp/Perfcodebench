def run(scores: list[int], k: int, iters: int) -> int:
    import heapq
    h = 0
    for _ in range(iters):
        top_k = heapq.nlargest(k, enumerate(scores), key=lambda x: x[1])
        h = 1469598103934665603
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h