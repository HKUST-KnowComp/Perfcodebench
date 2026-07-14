import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate indices to avoid repeated enumeration
    indexed_scores = [(s, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # Use nsmallest to get top k in O(N log k) time
        # To match the original sort key (-score, index), we use ((-s, i) for s, i in indexed_scores)
        top_k = heapq.nsmallest(k, indexed_scores, key=lambda x: (-x[0], x[1]))
        
        h = 1469598103934665603
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h