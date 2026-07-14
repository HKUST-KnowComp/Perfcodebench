import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    """
    Optimized top-k selection and checksum calculation.
    
    The goal is to select the top k items based on score (descending) 
    and index (ascending for ties), then compute a 64-bit checksum.
    """
    # Constants for the FNV-1a-like 64-bit hash
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK_64 = (1 << 64) - 1

    h = 0
    # Pre-enumerate to avoid repeated work if iters > 1
    # However, we must ensure we follow the logic of the benchmark harness.
    indexed_scores = list(enumerate(scores))

    for _ in range(iters):
        # heapq.nlargest is O(N log k). 
        # Key (x[1], -x[0]) ensures:
        # 1. Primary sort by score (x[1]) descending.
        # 2. Secondary sort by index (x[0]) ascending (since -x[0] is maximized).
        top_k = heapq.nlargest(k, indexed_scores, key=lambda x: (x[1], -x[0]))
        
        h = FNV_OFFSET_BASIS
        for index, score in top_k:
            # Checksum logic: h = (h ^ value) * prime
            # value = (score << 32) ^ index
            h ^= ((score << 32) ^ index)
            h = (h * FNV_PRIME) & MASK_64
            
    return h