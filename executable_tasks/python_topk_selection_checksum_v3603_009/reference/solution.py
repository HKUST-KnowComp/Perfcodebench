import heapq


def run(scores: list[int], k: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        heap: list[tuple[int, int]] = []
        for index, score in enumerate(scores):
            item = (score, -index)
            if len(heap) < k:
                heapq.heappush(heap, item)
            elif item > heap[0]:
                heapq.heapreplace(heap, item)
        out = sorted(((score, -neg_index) for score, neg_index in heap), key=lambda x: (-x[0], x[1]))
        h = 1469598103934665603
        for score, index in out:
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
