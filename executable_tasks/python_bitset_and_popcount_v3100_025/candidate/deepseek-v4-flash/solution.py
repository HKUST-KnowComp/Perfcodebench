def run(left: list[int], right: list[int], iters: int) -> int:
    # Precompute population counts for each pair to avoid recomputation across iterations
    popcounts = [(a & b).bit_count() for a, b in zip(left, right)]
    total = 0
    for _ in range(iters):
        total = sum(popcounts)
    return total