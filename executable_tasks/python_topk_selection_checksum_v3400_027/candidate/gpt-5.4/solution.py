def run(scores: list[int], k: int, iters: int) -> int:
    import heapq

    n = len(scores)
    if k <= 0:
        h = 1469598103934665603
        for _ in range(iters):
            pass
        return h

    if k >= n:
        ordered = sorted(enumerate(scores), key=lambda x: (-x[1], x[0]))
        h = 1469598103934665603
        mask = (1 << 64) - 1
        for _ in range(iters):
            h = 1469598103934665603
            for index, score in ordered:
                h ^= ((score << 32) ^ index)
                h = (h * 1099511628211) & mask
        return h

    mask = (1 << 64) - 1

    def key_of(index: int):
        score = scores[index]
        return score, -index

    h = 1469598103934665603
    for _ in range(iters):
        top_idx = heapq.nlargest(k, range(n), key=key_of)
        top_idx.sort(key=lambda i: (-scores[i], i))

        h = 1469598103934665603
        for index in top_idx:
            score = scores[index]
            h ^= ((score << 32) ^ index)
            h = (h * 1099511628211) & mask
    return h
