import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate pairs to avoid repeated generator overhead in the loop
    # We use (-score, index) to simulate the sorting key (-x[0], x[1])
    # because heapq.nlargest uses the natural order of the elements.
    # However, since we want the top-k based on (-score, index),
    # we can use a custom key or transform the data.
    
    # To match the baseline's sorting key: key=lambda x: (-x[0], x[1])
    # This means higher score first, then lower index first.
    # heapq.nlargest(k, iterable, key=...) is efficient.
    
    # Pre-creating the indexed list to avoid enumerate() in every iteration
    indexed_scores = list(enumerate(scores))
    
    h = 0
    for _ in range(iters):
        # nlargest is O(N log k), which is significantly faster than O(N log N) for k << N
        winners = heapq.nlargest(
            k, 
            indexed_scores, 
            key=lambda x: (-x[1], x[0])
        )
        
        h = 1469598103934665603
        mask = (1 << 64) - 1
        for index, score in winners:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
            
    return h