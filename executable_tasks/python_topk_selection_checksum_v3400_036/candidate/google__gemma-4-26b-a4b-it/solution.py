import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate the transformed tuples to avoid repeated work in the loop
    # We use (-score, index) so that the 'smallest' tuples are the ones we want
    # (highest score, lowest index).
    transformed = [(-s, i) for i, s in enumerate(scores)]
    
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), much faster than sorted() which is O(N log N)
        top_k_transformed = heapq.nsmallest(k, transformed)
        
        h = 1469598103934665603
        for neg_score, index in top_k_transformed:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h