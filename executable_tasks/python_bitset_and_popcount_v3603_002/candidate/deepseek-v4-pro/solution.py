def run(left: list[int], right: list[int], iters: int) -> int:
    return sum((a & b).bit_count() for a, b in zip(left, right)) * iters
