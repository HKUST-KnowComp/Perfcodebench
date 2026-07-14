def run(left: list[int], right: list[int], iters: int) -> int:
    # Precompute the AND of each pair once; they don't change across iterations.
    and_words = [a & b for a, b in zip(left, right)]
    total = 0
    for _ in range(iters):
        # Sum the popcount of all words efficiently.
        total = sum(map(int.bit_count, and_words))
    return total