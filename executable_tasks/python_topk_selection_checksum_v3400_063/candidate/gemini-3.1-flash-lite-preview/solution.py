import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate indexed scores to avoid repeated enumeration
    indexed_scores = [(-s, i) for i, s in enumerate(scores)]
    
    h = 0
    # FNV-1a constants
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    for _ in range(iters):
        # nsmallest is O(N log k), significantly faster than full sort for small k
        top_k = heapq.nsmallest(k, indexed_scores)
        
        h = FNV_OFFSET
        for neg_score, index in top_k:
            # Reconstruct original score: -neg_score
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * FNV_PRIME) & MASK
            
    return h