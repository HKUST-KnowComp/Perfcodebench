import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-compute pairs once: use negative score for max-heap via nsmallest
    pairs = [(-s, i) for i, s in enumerate(scores)]
    
    # Use heapq.nsmallest for O(n + k log k) instead of O(n log n) sort
    nsmallest = heapq.nsmallest
    
    # Constants
    INIT = 1469598103934665603
    MUL = 1099511628211
    MASK = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        top_k = nsmallest(k, pairs)
        h = INIT
        for neg_score, index in top_k:
            h ^= ((-neg_score) << 32) ^ index
            h = (h * MUL) & MASK
    return h
