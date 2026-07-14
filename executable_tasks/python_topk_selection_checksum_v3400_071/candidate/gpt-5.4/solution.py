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

        it = iter(enumerate(scores))
        heap = []
        push = heapq.heappush
        replace = heapq.heapreplace

        for _ in range(k):
            i, s = next(it)
            push(heap, (s, -i))

        for i, s in it:
            t = (s, -i)
            if t > heap[0]:
                replace(heap, t)

        ordered = [ -ni for _, ni in heap ]
        ordered.sort(key=lambda i: (-scores[i], i))

    mask = (1 << 64) - 1
    offset = 1469598103934665603
    prime = 1099511628211
    h = offset
    for _ in range(iters):
        h = offset
        for i in ordered[:k]:
            h ^= ((scores[i] << 32) ^ i)
            h = (h * prime) & mask
    return h
