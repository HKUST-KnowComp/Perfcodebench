import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-create tuples to avoid repeated generator overhead
    # We use (-score, index) to leverage heapq.nsmallest for a descending sort on score
    # and ascending sort on index.
    data = [(-s, i) for i, s in enumerate(scores)]
    
    # Localize constants for performance
    HASH_INIT = 1469598103934665603
    HASH_MULT = 1099511628211
    MASK = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), which is significantly faster than sorted() O(N log N)
        # when k << N.
        winners = heapq.nsmallest(k, data)
        
        h = HASH_INIT
        for neg_score, index in winners:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * HASH_MULT) & MASK
            
    return h