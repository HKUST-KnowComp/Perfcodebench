import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        # Use heapq.nlargest for efficient top-k selection
        # Sort by (-score, index) to match the original ordering
        pairs = heapq.nlargest(k, enumerate(scores), key=lambda x: (x[1], -x[0]))
        
        h = 1469598103934665603
        for index, score in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h