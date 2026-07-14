import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate constants to avoid repeated bitwise operations in the loop
    MASK64 = (1 << 64) - 1
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211

    # Pre-enumerate to avoid repeated enumerate calls if possible, 
    # but for memory efficiency we use a generator.
    # The key for sorting is (-score, index) to match the original logic.
    
    for _ in range(iters):
        # heapq.nsmallest is O(N log k), much faster than sorted() O(N log N) for small k
        # We use a key that mimics the (-score, index) sorting requirement
        top_k = heapq.nsmallest(
            k, 
            enumerate(scores), 
            key=lambda x: (-x[1], x[0])
        )
        
        h = FNV_OFFSET
        for index, score in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * FNV_PRIME) & MASK64
            
    return h