import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    n = len(scores)
    
    if k <= 0:
        for _ in range(iters):
            h = 1469598103934665603
        return h
    
    if k >= n:
        # Need all elements, just sort once and reuse
        pairs = sorted(((s, i) for i, s in enumerate(scores)), key=lambda x: (-x[0], x[1]))
        for _ in range(iters):
            h = 1469598103934665603
            for score, index in pairs:
                h ^= ((score << 32) ^ index)
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        return h
    
    # Use nlargest with negated index for tie-breaking (smaller index wins)
    # nlargest returns largest elements, so we use (score, -index) tuples
    # Then sort the k elements for final ordering
    nlargest = heapq.nlargest
    
    for _ in range(iters):
        # Get top-k by score (with -index for tie-breaking: larger -index means smaller index)
        top_k = nlargest(k, ((s, -i) for i, s in enumerate(scores)))
        # Sort by (-score, index) which is (-score, -(-i)) = (-score, i)
        top_k.sort(key=lambda x: (-x[0], -x[1]))
        
        h = 1469598103934665603
        for score, neg_index in top_k:
            index = -neg_index
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    
    return h
