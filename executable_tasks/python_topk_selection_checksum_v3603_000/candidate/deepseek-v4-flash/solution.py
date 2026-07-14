import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Compute top-k pairs (score, index) once, in required order
    pairs = [(s, i) for i, s in enumerate(scores)]
    top_k = heapq.nlargest(k, pairs, key=lambda x: (-x[0], x[1]))

    # Replicate FNV-1a hash computation iters times
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for score, index in top_k:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h