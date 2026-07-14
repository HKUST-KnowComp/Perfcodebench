import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Step 1: compute the ordered top-k pairs once
    # Use heapq.nsmallest to get (index, score) for the smallest (-score, index) keys
    # This gives the top-k in order: descending score, then ascending index
    top_k_pairs = heapq.nsmallest(k, enumerate(scores), key=lambda x: (-x[1], x[0]))
    # Convert to list of (score, index) as used in the checksum
    pairs = [(score, index) for index, score in top_k_pairs]
    # Step 2: iterate iters times, recomputing only the checksum
    for _ in range(iters):
        h = 1469598103934665603
        for score, index in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
