def run(left: list[int], right: list[int], iters: int) -> int:
    bc = int.bit_count
    total = 0
    for _ in range(iters):
        total = 0
        for a, b in zip(left, right):
            total += bc(a & b)
    return total