import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the tuples to avoid repeated enumeration and tuple creation in the loop
    # We use (-score, index) to allow heapq.nsmallest to find the 'smallest' values,
    # which correspond to the highest scores and lowest indices.
    data = [(-s, i) for i, s in enumerate(scores)]
    
    h_init = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    
    # Optimization: If k is very close to N, sorting is faster than heap
    if k > len(scores) // 2:
        def get_winners():
            # Sort by -score, then index
            sorted_data = sorted(data)
            return sorted_data[:k]
        get_winners = get_winners
    else:
        def get_winners():
            # Use heap for O(N log k) selection
            return heapq.nsmallest(k, data)
        get_winners = get_winners

    final_h = 0
    for _ in range(iters):
        winners = get_winners()
        h = h_init
        for neg_score, index in winners:
            score = -neg_score
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
        final_h = h
        
    return final_h