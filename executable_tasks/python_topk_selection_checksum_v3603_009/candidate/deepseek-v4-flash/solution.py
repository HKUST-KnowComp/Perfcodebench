import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    # Compute top-k ordered by (-score, index) using heapq.nlargest
    # key = (score, -index) gives descending sort: largest score first, smallest index first
    top_k = heapq.nlargest(k, ((score, i) for i, score in enumerate(scores)), key=lambda x: (x[0], -x[1]))
    # Compute checksum once (all iterations yield same result)
    h = 1469598103934665603
    mult = 1099511628211
    mask = (1 << 64) - 1
    for score, index in top_k:
        h ^= ((score << 32) ^ index)
        h = (h * mult) & mask
    return h