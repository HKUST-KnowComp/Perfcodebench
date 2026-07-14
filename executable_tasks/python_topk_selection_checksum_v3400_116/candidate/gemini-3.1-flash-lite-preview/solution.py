import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate packed values to avoid tuple overhead during selection
    # We want top scores (descending) and then lowest index (ascending).
    # Packing: (-score << 32) | index allows a single integer comparison.
    packed = [(-s << 32) | i for i, s in enumerate(scores)]
    
    h = 0
    for _ in range(iters):
        # Use nsmallest to get the k smallest packed values (which correspond to top scores)
        top_k_packed = heapq.nsmallest(k, packed)
        
        h = 1469598103934665603
        for val in top_k_packed:
            score = -(val >> 32)
            index = val & 0xFFFFFFFF
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h