from heapq import nlargest

def run(scores: list[int], k: int, iters: int) -> int:
    # Match baseline behavior for zero iterations
    if iters == 0:
        return 0

    n = len(scores)
    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    prime = 1099511628211
    mask = 0xFFFFFFFFFFFFFFFF

    # When k <= 0, no winners are processed but hash is reset each iter
    if k <= 0 or n == 0:
        return h

    sc = scores  # local binding for speed

    # Determine winners once (baseline redoes the same work per iter)
    if k >= n:
        # Full sort for all indices when k >= n
        idxs = sorted(range(n), key=lambda i: (-sc[i], i))
    else:
        # Select top-k by (score desc, index asc)
        idxs = nlargest(k, range(n), key=lambda i: (sc[i], -i))

    # Compute checksum over ordered winners
    for i in idxs[:k]:
        s = sc[i]
        h ^= ((s << 32) ^ i)
        h = (h * prime) & mask

    return h