import heapq

_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0:
        return _FNV_OFFSET

    if k >= n:
        winners = sorted(enumerate(scores), key=lambda t: (-t[1], t[0]))
        h = _FNV_OFFSET
        for _ in range(iters):
            h = _FNV_OFFSET
            for index, score in winners:
                h ^= ((score << 32) ^ index)
                h = (h * _FNV_PRIME) & _MASK64
        return h

    # Keep a min-heap of the current top-k items using the natural ordering
    # where worse items compare smaller: (score, -index).
    # This preserves the exact ranking rule of sorting by (-score, index).
    heap = []
    push = heapq.heappush
    replace = heapq.heapreplace

    for i, s in enumerate(scores[:k]):
        push(heap, (s, -i))

    for i in range(k, n):
        s = scores[i]
        item = (s, -i)
        if item > heap[0]:
            replace(heap, item)

    # Order winners exactly as required: score descending, index ascending.
    winners = sorted(heap, key=lambda t: (-t[0], -t[1]))

    h = _FNV_OFFSET
    for _ in range(iters):
        h = _FNV_OFFSET
        for score, neg_index in winners:
            index = -neg_index
            h ^= ((score << 32) ^ index)
            h = (h * _FNV_PRIME) & _MASK64
    return h
