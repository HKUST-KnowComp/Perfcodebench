import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    # Build list of (score, index) pairs
    if k >= n:
        # All items are winners
        winners = sorted(((s, i) for i, s in enumerate(scores)),
                         key=lambda x: (-x[0], x[1]))
    else:
        # Select top k by (-score, index) using heap, then sort to get exact order
        pairs = ((score, index) for index, score in enumerate(scores))
        winners = heapq.nlargest(k, pairs, key=lambda x: (-x[0], x[1]))
        winners.sort(key=lambda x: (-x[0], x[1]))

    # Compute hash (same formula as baseline) – repeated iters times
    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for score, index in winners:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
