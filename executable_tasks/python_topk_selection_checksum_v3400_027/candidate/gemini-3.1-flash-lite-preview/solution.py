import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the indexed list to avoid repeated enumeration
    indexed_scores = [(-s, i) for i, s in enumerate(scores)]
    h = 0
    for _ in range(iters):
        # Use nsmallest to get top k in O(N log K) time
        # Since we stored (-score, index), nsmallest naturally picks the largest scores
        top_k = heapq.nsmallest(k, indexed_scores)
        
        h = 1469598103934665603
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h