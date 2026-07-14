import heapq


def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k > n:
        k = n

    rng = range(n)
    nlargest = heapq.nlargest
    mask = (1 << 64) - 1
    offset = n
    scores_local = scores

    h = 1469598103934665603
    for _ in range(iters):
        # Exact ordering required: score descending, index ascending.
        # nlargest returns items in descending key order, so use key
        # (score, n - index) to prefer smaller indices on ties.
        top_idx = nlargest(k, rng, key=lambda i, s=scores_local, off=offset: (s[i], off - i))

        h = 1469598103934665603
        for i in top_idx:
            h ^= ((scores_local[i] << 32) ^ i)
            h = (h * 1099511628211) & mask
    return h
