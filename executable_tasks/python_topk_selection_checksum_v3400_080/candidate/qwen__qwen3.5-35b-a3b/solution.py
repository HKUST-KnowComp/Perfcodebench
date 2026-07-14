import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    
    # Use heapq.nlargest for O(N log k) selection instead of O(N log N) sort
    # enumerate yields (index, score), key=lambda x: x[1] sorts by score
    # nlargest is stable, preserving index order for ties (matches baseline)
    top_k = heapq.nlargest(k, enumerate(scores), key=lambda x: x[1])
    
    h = 1469598103934665603
    for index, score in top_k:
        h ^= ((score << 32) ^ index)
        h = (h * 1099511628211) & ((1 << 64) - 1)
    
    return h