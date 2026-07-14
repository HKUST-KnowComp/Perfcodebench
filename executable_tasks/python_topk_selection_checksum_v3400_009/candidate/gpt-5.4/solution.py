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

    h = 1469598103934665603
    mask = (1 << 64) - 1
    offset = 1 << 63
    local_scores = scores

    for _ in range(iters):
        if k == n:
            winners = sorted(range(n), key=lambda i: (-local_scores[i], i))
        else:
            # Keep a min-heap of the current best k items using a key where
            # larger is better: (score, -index). This exactly matches the
            # desired final ordering by (-score, index).
            heap = []
            heappush = heapq.heappush
            heapreplace = heapq.heapreplace

            for i, s in enumerate(local_scores):
                item = (s, -i, i)
                if len(heap) < k:
                    heappush(heap, item)
                elif item > heap[0]:
                    heapreplace(heap, item)

            # Sort winners into required output order: score descending, index ascending.
            heap.sort(reverse=True)
            winners = [i for _, __, i in heap]

        h = 1469598103934665603
        for i in winners:
            s = local_scores[i]
            h ^= (((s + offset) & mask) << 32) ^ i
            h = (h * 1099511628211) & mask
    return h
