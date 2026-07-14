import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0

    # top-k selection using heap for O(N log k)
    top = heapq.nlargest(k, enumerate(scores), key=lambda x: (x[1], -x[0]))

    MASK = (1 << 64) - 1
    MULT = 1099511628211
    h = 1469598103934665603
    for index, score in top:
        h ^= ((score << 32) ^ index)
        h = (h * MULT) & MASK

    return h