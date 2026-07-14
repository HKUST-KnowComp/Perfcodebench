import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if iters <= 0:
        return 0
    h = 1469598103934665603
    for index, score in heapq.nlargest(k, enumerate(scores), key=lambda x: (-x[1], x[0])):
        h ^= ((score << 32) ^ index)
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h