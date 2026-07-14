import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the tuples to avoid repeated enumeration and tuple creation in the loop
    # We use (-score, index) to allow heapq.nsmallest to find the 'smallest' which 
    # corresponds to the highest score and lowest index.
    data = [(-s, i) for i, s in enumerate(scores)]
    
    h_init = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211

    # We need to find the top k. Since we stored (-score, index),
    # the top k elements are the k smallest tuples in this list.
    # heapq.nsmallest is O(N log k).
    top_k_tuples = heapq.nsmallest(k, data)

    h = h_init
    for _ in range(iters):
        h = h_init
        for neg_score, index in top_k_tuples:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
    return h