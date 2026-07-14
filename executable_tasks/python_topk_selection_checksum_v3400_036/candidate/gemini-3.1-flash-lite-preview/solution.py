import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate indices to avoid repeated enumerate calls
    indexed_scores = list(enumerate(scores))
    
    for _ in range(iters):
        # Use nsmallest with a custom key to simulate the original sort order:
        # Original: sorted by (-score, index)
        # nsmallest: finds smallest elements based on key
        # To get top scores (descending) and indices (ascending), 
        # we use key=lambda x: (-scores[x[0]], x[0])
        top_k = heapq.nsmallest(k, indexed_scores, key=lambda x: (-x[1], x[0]))
        
        h = 1469598103934665603
        for index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h