import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    pairs = [(s, i) for i, s in enumerate(scores)]
    k = min(k, len(pairs))
    h = 0
    for _ in range(iters):
        topk = heapq.nlargest(k, pairs, key=lambda x: (x[0], -x[1]))
        h = 1469598103934665603
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h