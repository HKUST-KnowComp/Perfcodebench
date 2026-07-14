def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k > n:
        k = n

    mask = (1 << 64) - 1
    offset = 1 << 31

    # Fast path: if all items are needed, avoid heap/nlargest overhead and sort once.
    if k == n:
        order = sorted(range(n), key=lambda i: (-scores[i], i))
        h = 1469598103934665603
        for _ in range(iters):
            h = 1469598103934665603
            for i in order:
                h ^= ((scores[i] << 32) ^ i)
                h = (h * 1099511628211) & mask
        return h

    import heapq

    # Build comparable keys using only integers for speed.
    # key = ((score + offset) << 32) - index
    # Larger key means better item under ordering: score desc, index asc.
    # This preserves exact ordering for 32-bit-range scores.
    keys = [0] * n
    for i, s in enumerate(scores):
        keys[i] = ((s + offset) << 32) - i

    top_idx = heapq.nlargest(k, range(n), key=keys.__getitem__)
    top_idx.sort(key=lambda i: (-scores[i], i))

    h = 1469598103934665603
    for _ in range(iters):
        h = 1469598103934665603
        for i in top_idx:
            h ^= ((scores[i] << 32) ^ i)
            h = (h * 1099511628211) & mask
    return h
