from heapq import nlargest

def run(scores: list[int], k: int, iters: int) -> int:
    # Preserve semantics: if no iterations, return initial value (0 in baseline)
    if iters <= 0:
        return 0

    OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK = (1 << 64) - 1

    # If k <= 0, hashing loop is empty but h is reset to OFFSET each iteration.
    if k <= 0:
        return OFFSET

    n = len(scores)
    s = scores  # local binding for speed

    # Compute top-k indices once, preserving ordering: score desc, index asc
    if k >= n:
        # All items, fully ordered
        idxs = sorted(range(n), key=lambda i, s=s: (-s[i], i))
    else:
        # Partial selection: largest by (score, -index) gives desired order
        idxs = nlargest(k, range(n), key=lambda i, s=s: (s[i], -i))

    # Compute checksum once; repeated iterations would yield the same final h
    h = OFFSET
    for i in idxs:
        h ^= ((s[i] << 32) ^ i)
        h = (h * FNV_PRIME) & MASK

    return h
