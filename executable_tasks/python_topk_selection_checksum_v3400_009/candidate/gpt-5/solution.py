import heapq


def run(scores: list[int], k: int, iters: int) -> int:
    # If no iterations, baseline returns 0
    if iters <= 0:
        return 0

    n = len(scores)

    # Compute winners exactly once since scores do not change across iterations
    if k <= 0:
        winners = []
    elif k >= n:
        # Full ordering: score desc, index asc
        winners = sorted(range(n), key=lambda i: (-scores[i], i))
    else:
        # Partial selection: top-k by (score desc, index asc) using key (score, -index)
        winners = heapq.nlargest(k, range(n), key=lambda i: (scores[i], -i))

    # Compute checksum once; result is identical for each iteration, so final equals last iteration
    h = 1469598103934665603
    mask = (1 << 64) - 1
    s = scores  # local ref for speed
    for i in winners:
        v = (s[i] << 32) ^ i
        h ^= v
        h = (h * 1099511628211) & mask

    return h
