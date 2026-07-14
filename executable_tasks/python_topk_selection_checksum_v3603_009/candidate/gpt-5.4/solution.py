import heapq

_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0 or iters <= 0:
        return 0
    if k > n:
        k = n

    # Fast path: if all items are needed, avoid heap selection.
    if k == n:
        ordered = sorted(enumerate(scores), key=lambda x: (-x[1], x[0]))
        h = 0
        for _ in range(iters):
            h = _FNV_OFFSET
            for index, score in ordered:
                h ^= ((score << 32) ^ index)
                h = (h * _FNV_PRIME) & _MASK64
        return h

    # Keep a min-heap of the current top-k items using the natural ordering
    # (score, -index), so the worst winner is at the root.
    it = enumerate(scores)
    heap = []
    push = heapq.heappush
    replace = heapq.heapreplace

    for index, score in it:
        heap.append((score, -index))
        if len(heap) == k:
            break
    heapq.heapify(heap)

    for index, score in it:
        item = (score, -index)
        if item > heap[0]:
            replace(heap, item)

    # Final exact ordering required by correctness: score descending, index ascending.
    winners = sorted(heap, key=lambda x: (-x[0], -x[1]))

    h = 0
    for _ in range(iters):
        h = _FNV_OFFSET
        for score, neg_index in winners:
            h ^= ((score << 32) ^ (-neg_index))
            h = (h * _FNV_PRIME) & _MASK64
    return h
