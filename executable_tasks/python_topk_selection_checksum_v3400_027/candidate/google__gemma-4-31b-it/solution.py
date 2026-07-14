import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate constants
    MASK = (1 << 64) - 1
    MULTIPLIER = 1099511628211
    
    # To handle the tie-breaking rule (descending score, ascending index),
    # we store (score, -index). heapq.nlargest will pick the largest score,
    # and for ties, the largest -index (which is the smallest index).
    # This avoids the need for a custom key function in nlargest which is slower.
    
    # Pre-create the pairs list to avoid repeated generator overhead
    # We use -i to ensure that for equal scores, the smaller index is 'larger'
    pairs = [(s, -i) for i, s in enumerate(scores)]
    
    h = 0
    for _ in range(iters):
        # nlargest is O(N log k), significantly faster than sorted() O(N log N) for k << N
        winners = heapq.nlargest(k, pairs)
        
        h = 1469598103934665603
        for score, neg_index in winners:
            index = -neg_index
            h ^= ((score << 32) ^ index)
            h = (h * MULTIPLIER) & MASK
            
    return h