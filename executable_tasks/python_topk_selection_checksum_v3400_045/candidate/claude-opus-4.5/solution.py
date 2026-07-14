import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    n = len(scores)
    
    if k == 0:
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
    
    # Use nlargest with negated index for tie-breaking (smallest index wins)
    # heapq.nlargest returns largest elements, we want largest scores with smallest index as tiebreaker
    # Key: (score, -index) so larger score wins, and for same score, larger -index (smaller index) wins
    indexed_scores = [(s, -i, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # nlargest is O(n log k) vs O(n log n) for full sort
        top_k = heapq.nlargest(k, indexed_scores, key=lambda x: (x[0], x[1]))
        # Sort the k elements: by score desc, then index asc
        top_k.sort(key=lambda x: (-x[0], x[2]))
        
        h = 1469598103934665603
        for score, _, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    
    return h
