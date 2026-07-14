import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if k <= 0:
        return 1469598103934665603
    # Compute top-k pairs (score, index) in descending score, ascending index order
    topk = heapq.nsmallest(k, enumerate(scores), key=lambda x: (-x[1], x[0]))
    # Convert to list of (score, index) in required order
    pairs = [(score, index) for index, score in topk]
    h0 = 1469598103934665603
    h = h0
    for _ in range(iters):
        h = h0
        for score, index in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h