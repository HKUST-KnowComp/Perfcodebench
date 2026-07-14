def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        for a, b in zip(left, right):
            total += (a & b).bit_count()
    return total