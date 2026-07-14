def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        total = sum((a & b).bit_count() for a, b in zip(left, right))
    return total
