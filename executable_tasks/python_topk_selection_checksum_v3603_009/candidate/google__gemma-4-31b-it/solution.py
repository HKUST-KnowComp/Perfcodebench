import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the pairs to avoid repeated enumeration in the loop
    # The original sort key is (-score, index)
    pairs = [(-s, i) for i, s in enumerate(scores)]
    
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    h = 0
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), which is faster than sorted() O(N log N) for k << N
        winners = heapq.nsmallest(k, pairs)
        
        h = 1469598103934665603
        for neg_score, index in winners:
            score = -neg_score
            # Original checksum logic: h ^= ((score << 32) ^ index)
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
            
    return h