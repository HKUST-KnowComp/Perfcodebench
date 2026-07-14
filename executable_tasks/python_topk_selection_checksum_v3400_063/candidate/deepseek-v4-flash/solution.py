from heapq import nlargest

def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0 or k == 0:
        return 0
    # build pairs as (score, index) for keying
    pairs = [(s, i) for i, s in enumerate(scores)]
    # select top-k: descending score, then ascending index
    top = nlargest(k, pairs, key=lambda x: (x[0], -x[1]))
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for score, idx in top:
        h ^= ((score << 32) ^ idx)
        h = (h * 1099511628211) & mask
    return h