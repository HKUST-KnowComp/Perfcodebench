def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k >= n:
        ordered = sorted(range(n), key=lambda i: (-scores[i], i))
        h = 1469598103934665603
        for _ in range(iters):
            h = 1469598103934665603
            for i in ordered:
                h ^= ((scores[i] << 32) ^ i)
                h = (h * 1099511628211) & 18446744073709551615
        return h

    import heapq

    # Keep a min-heap of the current best k items using key (score, -index),
    # which corresponds to the worst item among the selected top-k at heap[0].
    heap = []
    push = heapq.heappush
    replace = heapq.heapreplace
    sc = scores

    for i, s in enumerate(sc):
        item = (s, -i)
        if len(heap) < k:
            push(heap, item)
        elif item > heap[0]:
            replace(heap, item)

    # Convert to required final order: score descending, index ascending.
    heap.sort(key=lambda x: (-x[0], -x[1]))

    h = 1469598103934665603
    for _ in range(iters):
        h = 1469598103934665603
        for s, neg_i in heap:
            h ^= ((s << 32) ^ (-neg_i))
            h = (h * 1099511628211) & 18446744073709551615
    return h
