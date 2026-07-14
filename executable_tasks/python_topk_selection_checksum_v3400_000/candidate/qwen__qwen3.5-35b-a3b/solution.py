import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if iters <= 0:
        return 0
    
    # Select top-k items efficiently using heapq.nlargest (O(N log k))
    # instead of full sort (O(N log N)).
    # Match baseline sort order: score descending, index ascending.
    # Generator yields (score, index).
    # Key for nlargest: (score, -index) ensures largest score, then smallest index.
    pairs = heapq.nlargest(k, ((s, i) for i, s in enumerate(scores)), key=lambda x: (x[0], -x[1]))
    
    h = 1469598103934665603
    for score, index in pairs:
        h ^= ((score << 32) ^ index)
        h = (h * 109951628211) & ((1 << 64) - 1)
            
    return h