import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # build tuple list once; negative score gives max-heap behaviour
    pairs = [(-s, i) for i, s in enumerate(scores)]
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mul = 1099511628211

    for _ in range(iters):
        topk = heapq.nsmallest(k, pairs)   # O(n log k)
        for negscore, idx in topk:
            h ^= ((-negscore) << 32) ^ idx
            h = (h * mul) & mask
    return h