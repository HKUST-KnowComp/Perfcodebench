def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    n = len(scores)
    # Get top-k indices sorted by (-score, index) using the key function
    indices = list(range(n))
    indices.sort(key=lambda i: (-scores[i], i))
    top_indices = indices[:k]
    # FNV-1a 64-bit hash constants
    h = 1469598103934665603
    mul = 1099511628211
    mask = (1 << 64) - 1
    sc = scores  # local alias for speed
    for i in top_indices:
        s = sc[i]
        h ^= ((s << 32) ^ i)
        h = (h * mul) & mask
    return h