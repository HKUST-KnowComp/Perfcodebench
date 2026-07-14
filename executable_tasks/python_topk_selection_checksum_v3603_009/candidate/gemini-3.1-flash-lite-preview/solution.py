import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate indices to avoid repeated enumerate calls
    indexed_scores = list(enumerate(scores))
    
    # Use a custom key for heapq: (-score, index)
    # This mimics the original sorting logic: descending score, ascending index
    def key_func(item):
        idx, score = item
        return (-score, idx)

    h = 0
    for _ in range(iters):
        # nsmallest is O(N log K), significantly faster than O(N log N) for small k
        top_k = heapq.nsmallest(k, indexed_scores, key=key_func)
        
        h = 1469598103934665603
        for idx, score in top_k:
            h ^= ((score << 32) ^ idx)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h