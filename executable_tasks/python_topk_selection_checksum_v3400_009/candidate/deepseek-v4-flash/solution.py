import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Build list of (score, index) tuples
    indexed_scores = [(s, i) for i, s in enumerate(scores)]
    # Get top-k: descending score, then ascending index
    topk = heapq.nlargest(k, indexed_scores, key=lambda x: (-x[0], x[1]))
    # Ensure exact order (nlargest already returns descending, but sort for safety)
    topk.sort(key=lambda x: (-x[0], x[1]))
    # Precompute the XOR values for the checksum
    values = [(score << 32) ^ index for score, index in topk]
    # Constants for the FNV-like checksum
    INIT = 1469598103934665603
    MULT = 1099511628211
    MASK = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = INIT
        for v in values:
            h ^= v
            h = (h * MULT) & MASK
    return h