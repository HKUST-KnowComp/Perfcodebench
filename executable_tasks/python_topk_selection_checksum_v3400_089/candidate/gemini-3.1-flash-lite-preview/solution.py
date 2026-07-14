import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate indices to avoid repeated enumeration
    indexed_scores = list(enumerate(scores))
    
    # Use a custom key for heapq to simulate (-score, index)
    # We use a tuple (-score, index) to ensure correct ordering
    def get_key(item):
        idx, score = item
        return (-score, idx)

    h = 0
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), significantly faster than full sort O(N log N)
        top_k = heapq.nsmallest(k, indexed_scores, key=get_key)
        
        h = 1469598103934665603
        for idx, score in top_k:
            h ^= ((score << 32) ^ idx)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h