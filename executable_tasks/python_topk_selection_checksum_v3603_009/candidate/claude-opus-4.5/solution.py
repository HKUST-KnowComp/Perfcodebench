import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    n = len(scores)
    
    # Pre-compute enumerated scores once
    # For heapq.nlargest with key, we need to get top-k by score (desc), then by index (asc) for ties
    # heapq.nlargest returns largest first, so we use score directly
    # For tie-breaking on index (ascending), we use -index in the key tuple
    
    # Create list of (score, index) pairs once
    indexed_scores = [(s, i) for i, s in enumerate(scores)]
    
    # Constants hoisted
    INIT_H = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    
    # Use nlargest with a key that sorts by (score desc, index asc)
    # nlargest finds largest, so for score we want high values = large
    # For index tie-break, we want low index = should come first = "larger" in nlargest terms
    # So key should be (score, -index)
    
    if k >= n:
        # If k >= n, just sort everything
        for _ in range(iters):
            pairs = sorted(indexed_scores, key=lambda x: (-x[0], x[1]))
            h = INIT_H
            for score, index in pairs[:k]:
                h ^= ((score << 32) ^ index)
                h = (h * MULT) & MASK
    else:
        # Use nlargest for partial selection, then sort the k elements
        nlargest = heapq.nlargest
        for _ in range(iters):
            # Get top-k by score (largest scores), with -index for tie-breaking
            top_k = nlargest(k, indexed_scores, key=lambda x: (x[0], -x[1]))
            # Sort the k elements by (-score, index) to get exact order
            top_k.sort(key=lambda x: (-x[0], x[1]))
            h = INIT_H
            for score, index in top_k:
                h ^= ((score << 32) ^ index)
                h = (h * MULT) & MASK
    return h
