from heapq import nlargest


def run(scores: list[int], k: int, iters: int) -> int:
    # Preserve baseline behavior: if no iterations, return initial h (0)
    if iters <= 0:
        return 0

    n = len(scores)
    # Compute effective number of winners to select, matching Python slicing semantics for negative k
    if k >= 0:
        m = k if k <= n else n
    else:
        m = n + k
        if m < 0:
            m = 0

    # FNV-1a 64-bit parameters
    h = 1469598103934665603
    if m <= 0:
        # With zero winners, baseline resets to offset basis and returns it (since iters > 0)
        return h

    # Fast paths based on m relative to n
    s = scores  # local alias

    if m == 1:
        # Single pass to find best index: max score, then min index on ties
        best_i = 0
        best_score = s[0] if n else 0
        # Start from 1 to avoid extra comparisons when n>0
        for i in range(1, n):
            si = s[i]
            if si > best_score or (si == best_score and i < best_i):
                best_score = si
                best_i = i
        idxs = [best_i]
    else:
        # Choose strategy: heap selection for small m, full sort otherwise
        if m * 64 < n:
            # nlargest returns in descending order by key
            idxs = nlargest(m, range(n), key=lambda i: (s[i], -i))
        else:
            # Full sort of indices by (-score, index)
            idxs = sorted(range(n), key=lambda i: (-s[i], i))[:m]

    mask = (1 << 64) - 1
    for i in idxs:
        val = ((s[i] << 32) ^ i)
        h ^= val
        h = (h * 1099511628211) & mask

    return h
