def run(scores: list[int], k: int, iters: int) -> int:
    # If no iterations requested, baseline returns 0
    if iters == 0:
        return 0

    # FNV-1a 64-bit offset basis and mask
    FNV_OFFSET = 1469598103934665603
    FNV_PRIME = 1099511628211
    MASK_64 = (1 << 64) - 1

    n = len(scores)
    if n == 0 or k <= 0:
        # No winners; baseline would set h to offset and not update it
        return FNV_OFFSET

    # Compute winners exactly once
    import heapq

    if k >= n:
        # Sort all indices by score desc, index asc
        winners = sorted(range(n), key=lambda i: (-scores[i], i))
    else:
        # Use nlargest with composite key: score desc, index asc achieved via (score, -index)
        winners = heapq.nlargest(k, range(n), key=lambda i: (scores[i], -i))

    # Compute checksum over ordered winners
    h = FNV_OFFSET
    s_list = scores  # local binding for speed
    for i in winners:
        s = s_list[i]
        h ^= ((s << 32) ^ i)
        h = (h * FNV_PRIME) & MASK_64

    return h