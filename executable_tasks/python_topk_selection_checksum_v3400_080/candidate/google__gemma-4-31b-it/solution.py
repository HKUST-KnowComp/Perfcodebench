import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the key function to avoid lambda overhead in the loop
    # We want max score (descending) and min index (ascending).
    # heapq.nsmallest finds the smallest elements. 
    # To get the 'top' (largest) scores, we negate the score.
    
    # Optimization: Use a generator expression with heapq.nsmallest
    # The key is (-score, index) to ensure correct ordering
    
    h_const = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    
    # Pre-calculate the data to avoid repeated enumerate calls if possible
    # However, since the loop is over iters, we must perform the selection inside
    indexed_scores = list(enumerate(scores))
    
    h = 0
    for _ in range(iters):
        # heapq.nsmallest is O(N log k)
        winners = heapq.nsmallest(k, indexed_scores, key=lambda x: (-x[1], x[0]))
        
        h = h_const
        for index, score in winners:
            h ^= ((score << 32) ^ index)
            h = (h * mult) & mask
            
    return h