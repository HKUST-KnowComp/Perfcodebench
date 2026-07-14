import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    items = [(-s, i) for i, s in enumerate(scores)]
    topk_neg = heapq.nsmallest(k, items)
    topk = [(-ns, i) for ns, i in topk_neg]
    
    h = 0
    FNV_PRIME = 1099511628211
    FNV_OFFSET = 1469598103934665603
    MASK = (1 << 64) - 1
    
    for _ in range(iters):
        h = FNV_OFFSET
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * FNV_PRIME) & MASK
    return h