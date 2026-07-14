import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate constants
    MASK = (1 << 64) - 1
    MULTIPLIER = 1099511628211
    INIT_H = 1469598103934665603

    # To handle the sorting criteria: score descending, index ascending
    # We can use (-score, index) as the key for a min-heap (nsmallest)
    # This allows us to find the k smallest (-score, index) pairs,
    # which corresponds to the k largest scores (and smallest indices for ties).
    
    h = 0
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), much faster than sorted() O(N log N) for small k
        # We create a generator to avoid allocating a full list of tuples before heapifying
        top_k = heapq.nsmallest(
            k, 
            (( -s, i) for i, s in enumerate(scores))
        )
        
        h = INIT_H
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * MULTIPLIER) & MASK
            
    return h