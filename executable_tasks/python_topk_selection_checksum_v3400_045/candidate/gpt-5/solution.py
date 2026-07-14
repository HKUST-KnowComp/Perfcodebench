from heapq import nlargest


def run(scores: list[int], k: int, iters: int) -> int:
    # If no iterations, baseline returns 0
    if iters == 0:
        return 0

    n = len(scores)
    # FNV-1a 64-bit constants
    offset = 1469598103934665603
    fnv_prime = 1099511628211
    mask = (1 << 64) - 1

    # If there are iterations but no items to hash (k <= 0 or empty scores),
    # the baseline sets h to the offset and performs no updates.
    if k <= 0 or n == 0:
        return offset

    s = scores

    # Select top-k indices by (score desc, index asc)
    # nlargest returns elements in descending order by the key
    indices = nlargest(k, range(n), key=lambda i, s=s: (s[i], -i))

    # Compute the hash for the selected winners once; identical across iterations
    h = offset
    for i in indices:
        h ^= ((s[i] << 32) ^ i)
        h = (h * fnv_prime) & mask

    return h
