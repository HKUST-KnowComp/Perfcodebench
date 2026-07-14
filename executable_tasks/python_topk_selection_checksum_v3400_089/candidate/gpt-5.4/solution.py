import heapq

_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        return _FNV_OFFSET
    if k >= n:
        ordered = sorted(enumerate(scores), key=lambda t: (-t[1], t[0]))
        h = _FNV_OFFSET
        for _ in range(iters):
            h = _FNV_OFFSET
            for idx, score in ordered:
                h ^= ((score << 32) ^ idx)
                h = (h * _FNV_PRIME) & _MASK64
        return h

    # Keep a min-heap of the current top-k items using the same ordering as the
    # final winners: better means higher score, and for ties lower index.
    # Heap key is (score, -index) so the worst current winner is at the root.
    heap = []
    push = heapq.heappush
    replace = heapq.heapreplace

    for idx, score in enumerate(scores):
        item = (score, -idx)
        if len(heap) < k:
            push(heap, item)
        elif item > heap[0]:
            replace(heap, item)

    # Convert to required output order: score descending, index ascending.
    winners = sorted(heap, key=lambda t: (-t[0], -t[1]))

    h = _FNV_OFFSET
    for _ in range(iters):
        h = _FNV_OFFSET
        for score, neg_idx in winners:
            idx = -neg_idx
            h ^= ((score << 32) ^ idx)
            h = (h * _FNV_PRIME) & _MASK64
    return h
