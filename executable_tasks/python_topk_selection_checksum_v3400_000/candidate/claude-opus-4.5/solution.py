import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-compute negated scores for min-heap to act as max-heap
    # Using (neg_score, index) so nsmallest gives top-k by score desc, index asc
    neg_pairs = [(-s, i) for i, s in enumerate(scores)]
    
    h = 0
    nsmallest = heapq.nsmallest
    
    for _ in range(iters):
        # nsmallest is O(n + k log n) vs O(n log n) for full sort
        topk = nsmallest(k, neg_pairs)
        
        # FNV-1a hash computation with inlined operations
        h = 1469598103934665603
        for neg_score, index in topk:
            h ^= ((-neg_score) << 32) ^ index
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    
    return h
