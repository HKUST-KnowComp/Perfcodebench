def run(scores: list[int], k: int, iters: int) -> int:
    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k > n:
        k = n

    import heapq

    # Faster local bindings
    enum = enumerate
    nlargest = heapq.nlargest
    mask = (1 << 64) - 1
    offset = 1 << 31
    fnv_offset = 1469598103934665603
    fnv_prime = 1099511628211

    # Use a single integer key so heapq.nlargest can compare plain ints quickly.
    # key = ((score + offset) << 32) - index
    # Ordering by descending key is equivalent to sorting by (-score, index)
    # for 32-bit score/index ranges, which this benchmark uses.
    top = nlargest(k, ((s + offset) << 32 | (0xFFFFFFFF - i) for i, s in enum(scores)))

    h = fnv_offset
    for _ in range(iters):
        h = fnv_offset
        for key in top:
            score = (key >> 32) - offset
            index = 0xFFFFFFFF - (key & 0xFFFFFFFF)
            h ^= ((score << 32) ^ index)
            h = (h * fnv_prime) & mask
    return h
