def run(left: list[int], right: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    total = sum((a & b).bit_count() for a, b in zip(left, right))
    return total
