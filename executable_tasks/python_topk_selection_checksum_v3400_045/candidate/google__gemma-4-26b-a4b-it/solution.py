import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h_init = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    
    # Pre-calculate the key function to avoid lambda overhead in the loop
    # We want descending score, then ascending index.
    # heapq.nsmallest finds the smallest elements. 
    # To get the 'top' elements (highest score), we can use a key that transforms 
    # the score to its negative.
    def get_key(item):
        idx, score = item
        return (-score, idx)

    # Pre-enumerate to avoid repeated work
    indexed_scores = list(enumerate(scores))
    
    h = 0
    for _ in range(iters):
        # Use nsmallest to get the top k elements in O(N log k)
        # The key (-score, index) ensures we get highest score first, then lowest index
        top_k = heapq.nsmallest(k, indexed_scores, key=lambda x: (-x[1], x[0]))
        
        h = h_init
        for index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
    return h