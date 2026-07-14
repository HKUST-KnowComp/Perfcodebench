def run(scores: list[int], k: int, iters: int) -> int:
    if iters == 0:
        return 0
    # Precompute the top-k elements sorted by (score descending, index ascending)
    sorted_pairs = sorted(((s, i) for i, s in enumerate(scores)), key=lambda x: (-x[0], x[1]))
    top_k = sorted_pairs[:k]
    # Precompute the XOR value for each top-k pair
    precomputed_vals = [(score << 32) ^ index for score, index in top_k]
    h_init = 1469598103934665603
    multiplier = 1099511628211
    mask = (1 << 64) - 1
    h = 0
    for _ in range(iters):
        h = h_init
        for val in precomputed_vals:
            h ^= val
            h = (h * multiplier) & mask
    return h
