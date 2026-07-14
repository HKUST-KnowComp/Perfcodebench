import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate tuples to avoid repeated generator overhead inside the loop
    # The original sort key is (-score, index)
    data = [(-s, i) for i, s in enumerate(scores)]
    
    h = 0
    for _ in range(iters):
        # heapq.nsmallest is more efficient than sorted()[:k] for small k
        # It uses a heap to maintain the k smallest elements
        winners = heapq.nsmallest(k, data)
        
        h = 1469598103934665603
        for neg_score, index in winners:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h