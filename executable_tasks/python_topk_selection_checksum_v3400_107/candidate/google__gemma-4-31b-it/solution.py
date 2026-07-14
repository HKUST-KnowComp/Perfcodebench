import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Pre-calculate pairs to avoid repeated enumerate/tuple creation in the loop
    # We use (-score, index) to simulate the sorting key (-x[0], x[1])
    # because heapq.nlargest uses the natural order of the elements.
    # However, since we want the largest scores and smallest indices,
    # we can use a custom key or transform the data.
    
    # To match the baseline's sorting key: key=lambda x: (-x[0], x[1])
    # This means we want the largest score, and for ties, the smallest index.
    # heapq.nlargest(k, range(len(scores)), key=lambda i: (scores[i], -i))
    # will give us the indices of the top k elements correctly.
    
    indices = list(range(len(scores)))
    
    h = 0
    for _ in range(iters):
        # Use nlargest with a key that mimics the baseline's sorting logic
        # Baseline: sorted by (-score, index). 
        # nlargest(k, ..., key=lambda i: (scores[i], -i)) returns the k indices
        # that have the largest (score, -index). This is equivalent to
        # the smallest (-score, index).
        top_k_indices = heapq.nlargest(k, indices, key=lambda i: (scores[i], -i))
        
        # The baseline sorts the pairs and takes the first k. 
        # nlargest returns them in descending order of the key.
        # Since the baseline's key is (-score, index), the 'smallest' are the winners.
        # nlargest(k, ..., key=lambda i: (scores[i], -i)) returns the 'largest' (score, -index),
        # which are exactly the winners in the correct order (descending score, ascending index).
        
        h = 1469598103934665603
        for idx in top_k_indices:
            score = scores[idx]
            h ^= ((score << 32) ^ idx)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
            
    return h