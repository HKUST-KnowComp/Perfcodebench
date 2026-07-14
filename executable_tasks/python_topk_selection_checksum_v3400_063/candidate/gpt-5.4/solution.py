import heapq

_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0:
        h = 0
        for _ in range(iters):
            h = _FNV_OFFSET
        return h

    if k >= n:
        winners = sorted(range(n), key=lambda i: (-scores[i], i))
    else:
        # Keep a min-heap of the current best k items using the same ordering
        # as the final result: better means higher score, then lower index.
        # Heap key is (score, -index) so the worst among the kept winners is at
        # the root: lower score is worse; for equal score, larger index is worse.
        heap = []
        push = heapq.heappush
        replace = heapq.heapreplace

        for i, s in enumerate(scores):
            item = (s, -i)
            if len(heap) < k:
                push(heap, item)
            elif item > heap[0]:
                replace(heap, item)

        winners = [-neg_i for _, neg_i in heap]
        winners.sort(key=lambda i: (-scores[i], i))

    h = 0
    scores_local = scores
    mask = _MASK64
    prime = _FNV_PRIME
    offset = _FNV_OFFSET
    for _ in range(iters):
        h = offset
        for i in winners:
            h ^= ((scores_local[i] << 32) ^ i)
            h = (h * prime) & mask
    return h
