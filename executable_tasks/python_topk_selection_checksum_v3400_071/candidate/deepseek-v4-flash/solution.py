import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    key = lambda item: (-item[0], item[1])
    for _ in range(iters):
        pairs = ((s, i) for i, s in enumerate(scores))
        winners = heapq.nsmallest(k, pairs, key=key)
        h = 1469598103934665603
        for score, index in winners:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
