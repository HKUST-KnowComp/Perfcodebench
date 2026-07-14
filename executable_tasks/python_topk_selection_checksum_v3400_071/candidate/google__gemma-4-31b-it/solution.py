import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate the top-k pairs once since scores and k are constant across iters
    # We want the largest scores, and for ties, the smallest index.
    # heapq.nsmallest with a key that negates the score and keeps index positive
    # effectively finds the top-k based on (-score, index).
    
    # To optimize, we create the pairs once.
    # The key lambda x: (-x[0], x[1]) is used to find the 'smallest' in terms of 
    # (negative score, index), which corresponds to the largest score and smallest index.
    
    pairs = heapq.nsmallest(k, range(len(scores)), key=lambda i: (-scores[i], i))
    
    # Extract the actual values to avoid repeated indexing in the loop
    winners = [(scores[i], i) for i in pairs]
    
    h_init = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    
    h = 0
    for _ in range(iters):
        h = h_init
        for score, index in winners:
            h ^= ((score << 32) ^ index)
            h = (h * multiplier) & mask
            
    return h