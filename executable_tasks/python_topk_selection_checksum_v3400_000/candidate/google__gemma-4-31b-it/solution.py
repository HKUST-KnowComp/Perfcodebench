import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate pairs to avoid repeated generator overhead
    # We use (-score, index) to simulate the sorting key (-x[0], x[1])
    # since heapq.nlargest uses the natural order of the elements.
    # However, heapq.nlargest is a max-heap based selection.
    # To get the top-k based on (-score, index), we need the smallest elements of (-score, index).
    # But heapq.nsmallest is more appropriate here.
    
    # To match the baseline: sorted(..., key=lambda x: (-x[0], x[1]))[:k]
    # This is equivalent to heapq.nsmallest(k, (( -s, i) for i, s in enumerate(scores)))
    
    # Optimization: Pre-create the list of tuples to avoid generator overhead in the loop
    data = [(-s, i) for i, s in enumerate(scores)]
    
    h = 0
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    initial_h = 1469598103934665603

    for _ in range(iters):
        # nsmallest is O(N log k), which is faster than sorted() O(N log N) for k << N
        winners = heapq.nsmallest(k, data)
        
        h = initial_h
        for neg_score, index in winners:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
            
    return h