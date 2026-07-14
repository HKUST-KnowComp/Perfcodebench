from heapq import nlargest

def run(scores: list[int], k: int, iters: int) -> int:
    # Match baseline behavior: if no iterations, return initial h (0)
    if iters == 0:
        return 0

    n = len(scores)
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK64 = (1 << 64) - 1

    # If k <= 0 or no scores, baseline resets to offset and performs no hashing
    if k <= 0 or n == 0:
        return FNV_OFFSET

    # Determine winners (indices) in the correct order: by score desc, index asc
    s = scores  # local binding for speed
    if k >= n:
        # Need all items in order; sort once with needed order
        order = sorted(range(n), key=lambda i, s=s: (-s[i], i))
    else:
        # Use heap-based top-k selection with tie-breaker via key
        order = nlargest(k, range(n), key=lambda i, s=s: (s[i], -i))

    # Compute checksum over ordered winners (exactly top-k items)
    h = FNV_OFFSET
    # order has length min(k, n) in both branches
    for i in order:
        x = ((s[i] << 32) ^ i)
        h ^= x
        h = (h * FNV_PRIME) & MASK64

    return h