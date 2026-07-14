def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        for a, b in zip(left, right):
            total += bin(a & b).count('1')
    return total