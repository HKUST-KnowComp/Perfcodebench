import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate the list of tuples to avoid repeated enumeration inside the loop
    # We use (negated_score, index) to allow heapq.nsmallest to find the 'top' elements
    # according to the original logic: highest score first, then lowest index.
    indexed_scores = [(-s, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # nsmallest on (-score, index) is equivalent to sorted(key=lambda x: (-x[0], x[1]))[:k]
        top_k = heapq.nsmallest(k, indexed_scores)
        
        h = 1469598103934665603
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h