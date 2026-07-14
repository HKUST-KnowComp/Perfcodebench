import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-compute negated scores for min-heap to act as max-heap
    # Tuple: (-score, index) so nsmallest gives highest scores, lowest index as tiebreaker
    neg_scores = [(-s, i) for i, s in enumerate(scores)]
    
    # Cache heapq.nsmallest locally
    nsmallest = heapq.nsmallest
    
    h = 0
    for _ in range(iters):
        # nsmallest is O(n + k log n) vs O(n log n) for full sort
        topk = nsmallest(k, neg_scores)
        
        h = 1469598103934665603
        for neg_score, index in topk:
            # Restore original score from negated value
            h ^= ((-neg_score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h
