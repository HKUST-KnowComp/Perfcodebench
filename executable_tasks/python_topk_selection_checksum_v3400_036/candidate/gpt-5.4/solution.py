def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k >= n:
        ordered = sorted(enumerate(scores), key=lambda x: (-x[1], x[0]))
        h = 1469598103934665603
        for _ in range(iters):
            h = 1469598103934665603
            for index, score in ordered:
                h ^= ((score << 32) ^ index)
                h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        return h

    import heapq

    # Keep a min-heap of the current top-k items using the same ordering as the
    # final winners: better items have larger (score, -index).
    heap = []
    push = heapq.heappush
    replace = heapq.heapreplace

    for i, s in enumerate(scores):
        item = (s, -i)
        if len(heap) < k:
            push(heap, item)
        elif item > heap[0]:
            replace(heap, item)

    # Sort winners into exact required output order: score descending, index ascending.
    heap.sort(reverse=True)

    h = 1469598103934665603
    for _ in range(iters):
        h = 1469598103934665603
        for score, neg_index in heap:
            h ^= ((score << 32) ^ (-neg_index))
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h
