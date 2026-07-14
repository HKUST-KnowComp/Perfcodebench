import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h_init = 1469598103934665603
    mask = (1 << 64) - 1
    
    # Pre-calculate pairs to avoid repeated enumeration and tuple creation inside the loop
    # We use ( -score, index ) to allow heapq.nsmallest to find the 'smallest' values
    # which correspond to the highest scores and lowest indices.
    indexed_scores = [(-s, i) for i, s in enumerate(scores)]
    
    h = h_init
    for _ in range(iters):
        # nsmallest is O(N log k), much faster than sorted() which is O(N log N)
        top_k_neg = heapq.nsmallest(k, indexed_scores)
        
        h = h_init
        for neg_score, index in top_k_neg:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
            
    return h