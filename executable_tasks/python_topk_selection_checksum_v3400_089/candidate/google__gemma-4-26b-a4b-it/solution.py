import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    # Pre-calculate constants for the FNV-1a style hash loop
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK_64 = (1 << 64) - 1

    # Pre-enumerate to avoid repeated work inside the loop if possible,
    # but since we need to sort by (-score, index), we create the tuples once.
    # Using a list of tuples is faster for repeated access than re-generating.
    indexed_scores = [(s, i) for i, s in enumerate(scores)]

    for _ in range(iters):
        # We want the top k based on (-score, index).
        # heapq.nsmallest is O(N log k), which is better than sorted() O(N log N) for small k.
        # The key function mimics the sorting logic: higher score first, then lower index.
        top_k = heapq.nsmallest(k, indexed_scores, key=lambda x: (-x[0], x[1]))
        
        h = FNV_OFFSET_BASIS
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * FNV_PRIME) & MASK_64
            
    return h