def run(scores: list[int], k: int, iters: int) -> int:
    import heapq
    
    h = 0
    n = len(scores)
    
    # Pre-build the pairs once if scores don't change
    pairs = [(s, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # Use heapq.nlargest with custom key for top-k selection
        # This is O(n log k) instead of O(n log n)
        top_k = heapq.nlargest(k, pairs, key=lambda x: (x[0], -x[1]))
        
        h = 1469598103934665603
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h