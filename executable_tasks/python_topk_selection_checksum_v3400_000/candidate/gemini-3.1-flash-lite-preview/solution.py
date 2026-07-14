import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate indices to avoid repeated enumerate calls
    indexed_scores = list(enumerate(scores))
    
    for _ in range(iters):
        # Use nsmallest to get top k in O(N log k) time
        # Criteria: primary descending score (-s), secondary ascending index (i)
        top_k = heapq.nsmallest(k, indexed_scores, key=lambda x: (-x[1], x[0]))
        
        h = 1469598103934665603
        for index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h