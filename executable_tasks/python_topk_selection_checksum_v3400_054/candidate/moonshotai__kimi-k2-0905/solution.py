import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    MASK = (1 << 64) - 1
    SEED = 1469598103934665603
    MUL = 1099511628211
    for _ in range(iters):
        # top-k pairs: (-score, index) to get descending score, ascending index
        topk = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)))
        h = SEED
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * MUL) & MASK
    return h