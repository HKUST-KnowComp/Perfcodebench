import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the enumerated list to avoid repeated enumeration inside the loop
    # We use a list of tuples (score, index) to avoid lambda overhead in the loop
    indexed_scores = [(s, i) for i, s in enumerate(scores)]
    
    # The sorting criteria is (-score, index). 
    # To use heapq.nsmallest efficiently, we can transform the data once
    # or use a key. Since we need to repeat this 'iters' times, 
    # we pre-transform the data to make the comparison direct.
    # Transform: (-score, index)
    transformed = [(-s, i) for s, i in indexed_scores]
    
    h_init = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        # Get the k smallest elements based on (-score, index)
        # This is O(N log k)
        top_k = heapq.nsmallest(k, transformed)
        
        h = h_init
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
    return h