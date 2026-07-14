import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    h_init = 1469598103934665603
    mask = (1 << 64) - 1
    
    # Pre-calculate the key function to avoid lambda overhead in the loop
    # We want to sort by (-score, index)
    # heapq.nsmallest finds the smallest elements. 
    # To get the 'top' scores, we use the tuple (-score, index).
    
    h = h_init
    for _ in range(iters):
        # Using nsmallest on a generator of tuples is O(N log k)
        # The key is to minimize object creation inside the loop
        winners = heapq.nsmallest(k, enumerate(scores), key=lambda x: (-x[1], x[0]))
        
        h = h_init
        for index, score in winners:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
    return h