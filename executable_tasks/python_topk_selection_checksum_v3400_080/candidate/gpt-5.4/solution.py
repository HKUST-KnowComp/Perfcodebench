def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k > n:
        k = n

    import heapq

    # Keep a min-heap of the current top-k items using (score, -index) so that
    # the worst winner is at the root: lowest score, and for ties the largest index.
    heap = []
    push = heapq.heappush
    replace = heapq.heapreplace

    for i, s in enumerate(scores):
        item = (s, -i)
        if len(heap) < k:
            push(heap, item)
        elif item > heap[0]:
            replace(heap, item)

    # Convert to required output order: score descending, index ascending.
    winners = sorted(heap, key=lambda x: (-x[0], -x[1]))

    h = 1469598103934665603
    mask = (1 << 64) - 1
    for _ in range(iters):
        h = 1469598103934665603
        for score, neg_index in winners:
            h ^= ((score << 32) ^ (-neg_index))
            h = (h * 1099511628211) & mask
    return h
