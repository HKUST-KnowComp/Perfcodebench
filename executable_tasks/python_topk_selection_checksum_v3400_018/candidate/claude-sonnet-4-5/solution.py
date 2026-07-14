import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    mask = (1 << 64) - 1
    prime = 1099511628211
    seed = 1469598103934665603
    
    for _ in range(iters):
        # Use heapq.nlargest for O(n log k) instead of O(n log n) sort
        # nlargest returns in descending order
        # For ties, we want smallest index first, so negate index in key
        pairs = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)), key=lambda x: (x[0], -x[1]))
        
        h = seed
        for score, index in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * prime) & mask
    
    return h
