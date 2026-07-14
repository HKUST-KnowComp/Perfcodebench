import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    FNV_PRIME = 1099511628211
    MASK_64 = (1 << 64) - 1
    INIT_HASH = 1469598103934665603
    
    for _ in range(iters):
        # Use heapq.nlargest for O(n log k) instead of O(n log n) sort
        # nlargest returns items in descending order
        pairs = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)), key=lambda x: (x[0], -x[1]))
        
        h = INIT_HASH
        for score, index in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * FNV_PRIME) & MASK_64
    
    return h
