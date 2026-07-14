from heapq import nlargest

def run(scores: list[int], k: int, iters: int) -> int:
    # Match baseline behavior: if iters == 0, nothing runs and h remains 0
    if iters <= 0:
        return 0

    # FNV-1a 64-bit constants
    FNV_OFFSET_BASIS = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    n = len(scores)

    if k <= 0:
        # With at least one iteration, baseline resets and returns the offset basis
        return FNV_OFFSET_BASIS

    if k >= n:
        # Full sort when k covers all items
        items = list(enumerate(scores))  # (index, score)
        items.sort(key=lambda t: (-t[1], t[0]))
        selected = items[:k]
    else:
        # Partial selection using heap for performance
        # Key ensures ordering: score descending, then index ascending via -index
        selected = nlargest(k, enumerate(scores), key=lambda t: (t[1], -t[0]))

    h = FNV_OFFSET_BASIS
    prime = FNV_PRIME
    mask = MASK64

    for idx, sc in selected:
        h ^= ((sc << 32) ^ idx)
        h = (h * prime) & mask

    return h
