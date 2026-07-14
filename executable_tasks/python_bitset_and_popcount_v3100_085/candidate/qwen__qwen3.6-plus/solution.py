def run(left: list[int], right: list[int], iters: int) -> int:
    mask = (1 << 64) - 1
    total = 0
    for a, b in zip(left, right):
        total += ((a & b) & mask).bit_count()
    return total * iters