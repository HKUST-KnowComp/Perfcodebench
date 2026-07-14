import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    mask = 0xFFFFFFFFFFFFFFFF
    prime = 1099511628211
    offset = 1469598103934665603
    h = 0
    for _ in range(iters):
        topk = heapq.nsmallest(k, ((-s, i) for i, s in enumerate(scores)))
        h = offset
        for neg_s, idx in topk:
            h = ((h ^ ((-neg_s << 32) ^ idx)) * prime) & mask
    return h
