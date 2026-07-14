import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h_init = 1469598103934665603
    mask = (1 << 64) - 1
    
    # Pre-calculate pairs to avoid repeated enumeration/tuple creation inside the loop
    # We use ( -score, index ) to allow heapq.nsmallest to find the 'largest' scores
    # because heapq is a min-heap. 
    # However, to keep it simple and fast, we can use a generator or list of tuples.
    indexed_scores = [(-s, i) for i, s in enumerate(scores)]
    
    h = h_init
    for _ in range(iters):
        # nsmallest on (-score, index) gives us the top k elements based on the criteria
        # (highest score first, then lowest index if scores tie)
        top_k = heapq.nsmallest(k, indexed_scores)
        
        h = h_init
        for neg_score, index in top_k:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
            
    return h