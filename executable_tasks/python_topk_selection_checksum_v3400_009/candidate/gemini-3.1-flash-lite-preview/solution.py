import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the items as tuples for efficient comparison
    # We want descending score, then ascending index.
    # heapq.nsmallest with a key allows us to simulate this efficiently.
    # To get descending score, we negate the score.
    items = [(s, i) for i, s in enumerate(scores)]
    
    h = 0
    for _ in range(iters):
        # nsmallest is O(N log k)
        top_k = heapq.nsmallest(k, items, key=lambda x: (-x[0], x[1]))
        
        h = 1469598103934665603
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h