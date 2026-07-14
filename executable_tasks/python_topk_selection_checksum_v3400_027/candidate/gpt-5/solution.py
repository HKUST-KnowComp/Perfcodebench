import heapq


def run(scores: list[int], k: int, iters: int) -> int:
    # If no iterations requested, baseline returns 0
    if iters == 0:
        return 0

    # FNV-1a 64-bit offset basis and prime
    h = 1469598103934665603
    mul = 1099511628211
    mask = (1 << 64) - 1

    n = len(scores)
    if k <= 0 or n == 0:
        return h

    sc = scores  # local reference for speed

    # Select top-k indices by (score desc, index asc)
    # heapq.nlargest returns results in the correct order by the provided key
    top_idx = heapq.nlargest(k, range(n), key=lambda i: (sc[i], -i))

    hx = h
    for i in top_idx:
        val = ((sc[i] << 32) ^ i)
        hx ^= val
        hx = (hx * mul) & mask

    return hx
