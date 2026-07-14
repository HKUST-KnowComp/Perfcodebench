import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h_init = 1469598103934665603
    mask = (1 << 64) - 1
    
    # Pre-calculate the items to avoid repeated enumeration/tuple creation in the loop
    # We store (-score, index) to use heapq.nsmallest to get the 'largest' scores
    # because heapq is a min-heap. 
    # The requirement is: sort by (-score, index). 
    # This means we want the smallest values of the tuple (-score, index).
    items = [(-s, i) for i, s in enumerate(scores)]
    
    h = h_init
    for _ in range(iters):
        # nsmallest is O(N log k), much faster than sorted() which is O(N log N)
        top_k = heapq.nsmallest(k, items)
        
        h = h_init
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
            
    return h