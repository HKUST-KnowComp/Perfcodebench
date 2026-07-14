def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        count = 0
        for a, b in zip(left, right):
            count += (a & b).bit_count()
        total = count
    return total
