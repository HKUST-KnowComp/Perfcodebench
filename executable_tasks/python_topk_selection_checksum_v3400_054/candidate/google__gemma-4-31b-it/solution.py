import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    h = 0
    
    # Pre-calculate the pairs to avoid repeated generator overhead inside the loop
    # We use (-score, index) to simulate the descending score, ascending index sort
    data = [(-s, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), which is faster than sorted() O(N log N) for k << N
        winners = heapq.nsmallest(k, data)
        
        h = 1469598103934665603
        for neg_score, index in winners:
            # Original score was -neg_score
            h ^= ((-neg_score << 32) ^ index)
            h = (h * multiplier) & mask
            
    return h