import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    """
    Optimized top-k selection and checksum calculation.
    Uses heapq.nlargest which is O(N log k) and implemented in C.
    The key (score, -index) ensures that for tied scores, the smaller index is selected.
    """
    # Constants for FNV-1a 64-bit hash
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = 0xFFFFFFFFFFFFFFFF

    h = 0
    # Pre-enumerate to avoid repeated work if possible, though scores might change in some contexts
    # but here we follow the baseline structure.
    for _ in range(iters):
        # heapq.nlargest is efficient for partial sorting.
        # We want highest score, then lowest index.
        # By using (score, -index) as the key, nlargest will pick the maximum score
        # and the maximum (-index), which corresponds to the minimum index.
        top_k = heapq.nlargest(k, enumerate(scores), key=lambda x: (x[1], -x[0]))
        
        h = FNV_OFFSET_BASIS
        for index, score in top_k:
            # Checksum calculation matching the baseline logic
            h ^= ((score << 32) | index)
            h = (h * FNV_PRIME) & MASK
            
    return h