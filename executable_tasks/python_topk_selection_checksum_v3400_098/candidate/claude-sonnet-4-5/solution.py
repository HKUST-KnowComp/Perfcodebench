import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-build negated pairs for faster heap operations
    neg_pairs = [(-s, i, s) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # Use nsmallest with pre-negated scores (equivalent to top-k largest)
        # Returns in ascending order of (-score, index), which gives us top-k by score
        top_k = heapq.nsmallest(k, neg_pairs)
        
        h = 1469598103934665603
        for _, index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h