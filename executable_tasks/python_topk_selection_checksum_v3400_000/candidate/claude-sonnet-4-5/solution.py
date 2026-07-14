import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    n = len(scores)
    
    # Precompute enumerated scores to avoid repeated enumeration
    indexed_scores = [(s, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # Use heapq.nlargest with custom key for top-k selection
        # This is O(n log k) instead of O(n log n)
        top_k = heapq.nlargest(k, indexed_scores, key=lambda x: (x[0], -x[1]))
        
        h = 1469598103934665603
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h