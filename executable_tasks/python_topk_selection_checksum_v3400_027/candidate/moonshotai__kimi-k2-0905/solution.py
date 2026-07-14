import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    mask = (1 << 64) - 1
    for _ in range(iters):
        # top-k pairs: (-score, index) to sort descending by score, ascending by index
        topk = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)))
        h = 1469598103934665603
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
    return h