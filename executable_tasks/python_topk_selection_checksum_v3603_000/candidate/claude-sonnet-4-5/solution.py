import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-build pairs with negated scores for efficient top-k selection
    pairs = [(-s, i, s) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # Use heapq.nsmallest to get top-k in O(n log k) time
        # The tuple comparison naturally sorts by (-score, index)
        top_k = heapq.nsmallest(k, pairs)
        
        h = 1469598103934665603
        for neg_score, index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h