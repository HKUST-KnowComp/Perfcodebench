def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        current_total = 0
        for a, b in zip(left, right):
            current_total += (a & b).bit_count()
        total = current_total
    return total