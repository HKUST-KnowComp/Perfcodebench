import heapq

MASK = (1 << 64) - 1
SEED = 1469598103934665603
MULT = 1099511628211

def run(scores: list[int], k: int, iters: int) -> int:
    if iters <= 0:
        return 0
    
    # Use nlargest for O(N log k) instead of O(N log N)
    # Key: (score, -index) to match sorted((-score, index)) behavior
    top_k = heapq.nlargest(
        k,
        ((s, i) for i, s in enumerate(scores)),
        key=lambda x: (x[0], -x[1])
    )
    
    h = SEED
    for score, index in top_k:
        h ^= ((score << 32) ^ index)
        h = (h * MULT) & MASK
    
    return h