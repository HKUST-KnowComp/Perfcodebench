import heapq
from typing import List

_MASK = (1 << 64) - 1
_FNV_MULT = 1099511628211


def run(scores: List[int], k: int, iters: int) -> int:
    h = 0
    for _ in range(iters):
        # Select top-k (largest score, smallest index) using a min-heap of size k
        topk = heapq.nsmallest(
            k,
            ((s, i) for i, s in enumerate(scores)),
            key=lambda x: (-x[0], x[1])
        )
        # Ensure ascending order of indices for deterministic checksum
        topk.sort(key=lambda x: (-x[0], x[1]))

        h = 1469598103934665603
        for score, index in topk:
            h ^= ((score << 32) ^ index)
            h = (h * _FNV_MULT) & _MASK
    return h