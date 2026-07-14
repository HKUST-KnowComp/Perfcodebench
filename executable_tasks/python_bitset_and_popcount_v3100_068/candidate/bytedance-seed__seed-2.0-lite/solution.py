def run(left: list[int], right: list[int], iters: int) -> int:
    total_popcount = sum((a & b).bit_count() for a, b in zip(left, right))
    return total_popcount * iters