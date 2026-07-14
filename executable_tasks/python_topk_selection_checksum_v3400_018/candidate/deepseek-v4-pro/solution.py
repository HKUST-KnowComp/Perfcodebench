def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    n = len(scores)
    # Decorate with negated score and index for fast tuple comparison
    decorated = [(-s, i, s) for i, s in enumerate(scores)]
    decorated.sort()
    limit = k if k < n else n
    h = 1469598103934665603
    mask = (1 << 64) - 1
    multiplier = 1099511628211
    for i in range(limit):
        t = decorated[i]
        score = t[2]
        index = t[1]
        h ^= ((score << 32) ^ index)
        h = (h * multiplier) & mask
    return h
