def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0:
        return 1469598103934665603

    if k > n:
        k = n

    import heapq

    mask = (1 << 64) - 1
    offset = 1 << 63

    # Precompute transformed keys once since scores do not change across iterations.
    # We need ordering by (-score, index), i.e. score descending then index ascending.
    # Using key = (score, -index) lets us keep a min-heap of the current top-k items,
    # where the smallest key is the worst among the selected winners.
    items = [(scores[i], -i, i) for i in range(n)]

    if k == n:
        winners = sorted(items, key=lambda t: (-t[0], t[2]))
    else:
        heap = items[:k]
        heapq.heapify(heap)
        for i in range(k, n):
            item = items[i]
            if item > heap[0]:
                heapq.heapreplace(heap, item)
        winners = sorted(heap, key=lambda t: (-t[0], t[2]))

    h = 0
    for _ in range(iters):
        h = 1469598103934665603
        for score, _neg_i, index in winners:
            h ^= (((score + offset) & mask) << 32) ^ index
            h = (h * 1099511628211) & mask
    return h
