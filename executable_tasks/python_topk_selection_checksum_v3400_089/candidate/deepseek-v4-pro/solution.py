import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Extract top-k items ordered by highest score, then lowest index.
    # Using nsmallest with key (-score, idx) gives the exact ordering required.
    top_k = heapq.nsmallest(k, ((-s, i, s) for i, s in enumerate(scores)))
    
    h = 1469598103934665603
    mask = (1 << 64) - 1
    mult = 1099511628211
    
    for _, idx, score in top_k:
        h ^= (score << 32) ^ idx
        h = (h * mult) & mask
    
    return h
