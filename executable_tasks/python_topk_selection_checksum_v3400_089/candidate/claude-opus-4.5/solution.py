def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h
    if k >= n:
        k = n
    
    h = 0
    for _ in range(iters):
        # Find k-th largest using partial sort approach
        # For small k, use heap-based selection; for larger k, use full sort
        if k * 10 < n:
            # Use a selection approach: find threshold, then filter and sort
            import heapq
            # Get k largest scores (not indices yet)
            largest_scores = heapq.nlargest(k, scores)
            threshold = largest_scores[-1]
            
            # Collect all candidates at or above threshold
            candidates = [(s, i) for i, s in enumerate(scores) if s >= threshold]
            # Sort by (-score, index) and take top k
            candidates.sort(key=lambda x: (-x[0], x[1]))
            pairs = candidates[:k]
        else:
            pairs = sorted(((s, i) for i, s in enumerate(scores)), key=lambda x: (-x[0], x[1]))[:k]
        
        h = 1469598103934665603
        for score, index in pairs:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h
