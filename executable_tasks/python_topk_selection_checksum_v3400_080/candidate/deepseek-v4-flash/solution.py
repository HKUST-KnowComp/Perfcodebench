import heapq

def run(scores: list[int], k: int, iters: int) -> int:
    # Compute top-k items once: sorted by score descending, index ascending.
    # Use heapq.nsmallest for O(n log k) complexity.
    # key: (-score, index) -> nsmallest yields ascending order, which is desired.
    top_k = heapq.nsmallest(k, enumerate(scores), key=lambda x: (-x[1], x[0]))
    # Convert to (score, index) tuples as used in original.
    ordered = [(score, idx) for idx, score in top_k]
    # Compute FNV-1a 64-bit checksum once.
    h = 1469598103934665603
    for score, index in ordered:
        h ^= ((score << 32) ^ index)
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h