import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    mask = (1 << 64) - 1
    for _ in range(iters):
        # top-k pairs: (-score, index) so that highest scores come first
        topk = heapq.nsmallest(k, ((s, i) for i, s in enumerate(scores)))
        h = 1469598103934665603
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
    return h