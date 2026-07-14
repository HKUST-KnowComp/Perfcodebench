import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate constants for the checksum
    MASK = (1 << 64) - 1
    PRIME = 1099511628211
    
    # To handle the tie-breaking rule (descending score, ascending index),
    # we store scores as positive and indices as negative for the heap,
    # or more simply, use a custom key. 
    # However, heapq.nlargest is most efficient with simple tuples.
    # We use (score, -index) so that nlargest picks the largest score,
    # and for equal scores, the largest (-index), which is the smallest index.
    
    # Pre-create the list of tuples to avoid repeated generator overhead
    # We use (score, -index) to ensure that nlargest picks the smallest index on tie
    data = [(s, -i) for i, s in enumerate(scores)]
    
    h = 0
    for _ in range(iters):
        # nlargest is O(N log k)
        winners = heapq.nlargest(k, data)
        
        h = 1469598103934665603
        for score, neg_index in winners:
            index = -neg_index
            h ^= ((score << 32) ^ index)
            h = (h * PRIME) & MASK
            
    return h