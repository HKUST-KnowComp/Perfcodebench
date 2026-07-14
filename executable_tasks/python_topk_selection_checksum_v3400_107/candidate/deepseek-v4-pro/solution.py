import heapq
from typing import List

def run(scores: List[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    if k > 0:
        # Get top-k pairs (score, index) in descending score, ascending index order
        top_pairs = [(score, idx) for idx, score in heapq.nlargest(
            k, enumerate(scores), key=lambda x: (x[1], -x[0])
        )]
    else:
        top_pairs = []
    h = 1469598103934665603
    for score, index in top_pairs:
        h ^= ((score << 32) ^ index)
        h = (h * 1099511628211) & ((1 << 64) - 1)
    return h
