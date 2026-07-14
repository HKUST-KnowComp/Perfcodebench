import heapq

_MASK64 = (1 << 64) - 1
_FNV_OFFSET = 1469598103934665603
_FNV_PRIME = 1099511628211


def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0 or n == 0:
        return _FNV_OFFSET

    if k >= n:
        ordered = sorted(enumerate(scores), key=lambda x: (-x[1], x[0]))
        h = _FNV_OFFSET
        for _ in range(iters):
            h = _FNV_OFFSET
            for index, score in ordered:
                h ^= ((score << 32) ^ index)
                h = (h * _FNV_PRIME) & _MASK64
        return h

    top = heapq.nlargest(k, enumerate(scores), key=lambda x: (x[1], -x[0]))
    top.sort(key=lambda x: (-x[1], x[0]))

    h = _FNV_OFFSET
    for _ in range(iters):
        h = _FNV_OFFSET
        for index, score in top:
            h ^= ((score << 32) ^ index)
            h = (h * _FNV_PRIME) & _MASK64
    return h
