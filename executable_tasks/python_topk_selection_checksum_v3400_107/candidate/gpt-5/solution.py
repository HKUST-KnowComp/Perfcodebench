from heapq import nlargest

def run(scores: list[int], k: int, iters: int) -> int:
    # Preserve baseline behavior when no iterations are requested
    if iters == 0:
        return 0

    n = len(scores)

    # Compute winners exactly once; same result would be produced each iteration
    if k <= 0 or n == 0:
        winners = []
    elif k >= n:
        # Sort all indices by (score desc, index asc)
        # Use combined integer key: higher score => larger, tie broken by smaller index => larger key via -index
        winners = sorted(range(n), key=lambda i: (scores[i] << 32) - i, reverse=True)
    elif k == 1:
        # Single pass to find the best index by (score desc, index asc)
        best_i = 0
        best_s = scores[0]
        for i in range(1, n):
            s = scores[i]
            if s > best_s or (s == best_s and i < best_i):
                best_s = s
                best_i = i
        winners = [best_i]
    else:
        # O(n log k) selection using heapq.nlargest with combined key
        # Key: (score << 32) - index ensures correct tie-breaking and ordering
        key = lambda i: (scores[i] << 32) - i
        winners = nlargest(k, range(n), key=key)

    # FNV-1a 64-bit hashing over ordered winners (score desc, index asc)
    h = 1469598103934665603
    mask = (1 << 64) - 1
    prime = 1099511628211
    for i in winners:
        s = scores[i]
        h ^= ((s << 32) ^ i)
        h = (h * prime) & mask
    return h
