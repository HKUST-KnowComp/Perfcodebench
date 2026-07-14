import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the items as (-score, index) to use min-heap for top-k
    # This allows us to use heapq.nsmallest which is O(N log K)
    items = [(-s, i) for i, s in enumerate(scores)]
    
    h = 0
    for _ in range(iters):
        # nsmallest returns the k smallest elements based on the tuple (-score, index)
        # which corresponds to the largest scores and smallest indices.
        top_k = heapq.nsmallest(k, items)
        
        h = 1469598103934665603
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h