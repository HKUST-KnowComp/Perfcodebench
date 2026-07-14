import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        pairs = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)), key=lambda x: (x[0], -x[1]))
        h = 1469598103934665603
        for score, index in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h