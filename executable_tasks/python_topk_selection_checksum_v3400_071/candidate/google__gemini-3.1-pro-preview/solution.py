import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if iters <= 0:
        return 0
    if k <= 0:
        return 1469598103934665603

    # Create tuples of (-score, index) to naturally sort by score descending, then index ascending.
    arr = [(-s, i) for i, s in enumerate(scores)]
    
    # Use heapq.nsmallest for partial selection if k is small relative to the list size.
    # Otherwise, full sort is generally faster due to Python's underlying C implementation (Timsort).
    if k * 10 < len(arr):
        top_k = heapq.nsmallest(k, arr)
    else:
        arr.sort()
        top_k = arr[:k]
        
    h = 1469598103934665603
    for neg_s, index in top_k:
        h ^= (((-neg_s) << 32) ^ index)
        h = (h * 1099511628211) & 18446744073709551615
        
    return h
