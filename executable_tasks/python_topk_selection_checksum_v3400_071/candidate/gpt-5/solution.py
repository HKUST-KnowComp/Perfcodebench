from heapq import nlargest


def run(scores: list[int], k: int, iters: int) -> int:
    # If no iterations (or negative), behavior matches baseline: return initial h (0)
    if iters <= 0:
        return 0

    n = len(scores)
    # For k <= 0, baseline sets h to offset basis and performs no item hashing
    if k <= 0 or n == 0:
        return 1469598103934665603

    # Effective k cannot exceed n
    k_eff = n if k > n else k

    sc = scores  # local binding for speed

    # Choose strategy based on k relative to n
    if k_eff > (n >> 1):
        # Large k: sort all indices by (-score, index)
        sc_local = sc
        winners_idx = sorted(range(n), key=lambda i, sc=sc_local: (-sc[i], i))
        if k_eff != n:
            winners_idx = winners_idx[:k_eff]
    else:
        # Small k: use nlargest over indices with key (score, -index)
        sc_local = sc
        winners_idx = nlargest(k_eff, range(n), key=lambda i, sc=sc_local: (sc[i], -i))

    # Compute 64-bit FNV-1a-like hash over ordered winners
    h = 1469598103934665603
    mask = (1 << 64) - 1
    for i in winners_idx:
        s = sc[i]
        h ^= ((s << 32) ^ i)
        h = (h * 1099511628211) & mask

    return h
