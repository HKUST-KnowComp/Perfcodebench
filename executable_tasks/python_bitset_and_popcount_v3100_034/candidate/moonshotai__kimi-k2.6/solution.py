def run(left: list[int], right: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    total = 0
    bc = int.bit_count
    for a, b in zip(left, right):
        total += bc(a & b)
    return total * iters
