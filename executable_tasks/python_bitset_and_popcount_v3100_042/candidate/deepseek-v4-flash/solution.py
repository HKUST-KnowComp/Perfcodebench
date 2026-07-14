def run(left: list[int], right: list[int], iters: int) -> int:
    # Precompute AND of each pair
    and_words = [a & b for a, b in zip(left, right)]
    total = 0
    for _ in range(iters):
        total = sum(w.bit_count() for w in and_words)
    return total