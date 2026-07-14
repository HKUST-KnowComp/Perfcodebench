import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the pairs to avoid repeated enumeration in the loop
    # We use (-score, index) to allow heapq.nlargest to handle the primary sort (descending score)
    # and secondary sort (ascending index) correctly.
    # However, heapq.nlargest is a max-heap based approach. 
    # To get top-k by score (desc) then index (asc), we can use a custom key.
    
    # The baseline uses: sorted(((s, i) for i, s in enumerate(scores)), key=lambda x: (-x[0], x[1]))
    # This means: Higher score first; if scores are equal, lower index first.
    
    # To achieve this with heapq.nlargest, we need a key that represents the priority.
    # Since we want max score and min index, we can use (score, -index).
    
    data = [(s, -i, i) for i, s in enumerate(scores)]
    
    h = 0
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    
    for _ in range(iters):
        # nlargest returns the k largest elements based on the tuple (score, -index, index)
        # This correctly implements: max score, then min index (because -index is larger for smaller index)
        winners = heapq.nlargest(k, data)
        
        h = 1469598103934665603
        for score, neg_idx, index in winners:
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
            
    return h