import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Precompute top-k winners ordered by (-score, index)
    # Enumerate gives (index, score). Key for nlargest: (score, -index)
    top = heapq.nlargest(k, enumerate(scores), key=lambda x: (x[1], -x[0]))
    winners = [(s, i) for i, s in top]

    # FNV-1a 64-bit constants
    FNV_offset_basis = 1469598103934665603
    FNV_prime = 1099511628211
    mask = (1 << 64) - 1

    h = 0
    for _ in range(iters):
        h = FNV_offset_basis
        for score, index in winners:
            h ^= ((score << 32) ^ index)
            h = (h * FNV_prime) & mask
    return h