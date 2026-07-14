import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate constants
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    
    # To mimic sorted(..., key=lambda x: (-x[0], x[1])), 
    # we want the smallest values of (-score, index).
    # heapq.nsmallest is O(N log k).
    
    h = 0
    for _ in range(iters):
        # We use a generator to avoid creating a full list of tuples before heap selection
        # The key is (-score, index) to get the top-k scores (descending) and then indices (ascending)
        winners = heapq.nsmallest(k, enumerate(scores), key=lambda x: (-x[1], x[0]))
        
        h = 1469598103934665603
        for index, score in winners:
            # Original logic: h ^= ((score << 32) ^ index)
            h ^= ((score << 32) ^ index)
            h = (h * MULT) & MASK
            
    return h