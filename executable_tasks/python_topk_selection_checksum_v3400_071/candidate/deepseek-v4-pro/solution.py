import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    if k <= 0:
        return 1469598103934665603
    if k > len(scores):
        k = len(scores)
    # create list of (index, score) to avoid repeated enumeration
    items = [(i, s) for i, s in enumerate(scores)]
    # select top k based on score descending, index ascending
    top_k = heapq.nlargest(k, items, key=lambda x: (x[1], -x[0]))
    # sort them according to the required order: (-score, index)
    top_k.sort(key=lambda x: (-x[1], x[0]))
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for idx, sc in top_k:
        h ^= ((sc << 32) ^ idx)
        h = (h * 1099511628211) & mask
    return h
