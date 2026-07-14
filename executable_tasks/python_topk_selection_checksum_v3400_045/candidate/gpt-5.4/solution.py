def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k >= n:
        ordered = sorted(range(n), key=lambda i: (-scores[i], i))
    else:
        import heapq
        # Keep a min-heap of the current top-k items using the same ordering
        # as the final winners: better means larger score, then smaller index.
        # Heap root is the worst among current winners.
        it = enumerate(scores)
        heap = []
        for _ in range(k):
            i, s = next(it)
            heap.append((s, -i))
        heapq.heapify(heap)
        for i, s in it:
            item = (s, -i)
            if item > heap[0]:
                heapq.heapreplace(heap, item)
        ordered = sorted((-ni for _, ni in heap), key=lambda i: (-scores[i], i))

    mask = (1 << 64) - 1
    h = 1469598103934665603
    for _ in range(iters):
        h = 1469598103934665603
        for i in ordered:
            h ^= ((scores[i] << 32) ^ i)
            h = (h * 1099511628211) & mask
    return h
