import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if k == 0:
        return 1469598103934665603
    # select top k: largest score first, smallest index first
    topk = heapq.nlargest(k, enumerate(scores), key=lambda x: (x[1], -x[0]))
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MUL = 1099511628211
    for index, score in topk:
        h ^= ((score << 32) ^ index)
        h = (h * MUL) & MASK
    return h
