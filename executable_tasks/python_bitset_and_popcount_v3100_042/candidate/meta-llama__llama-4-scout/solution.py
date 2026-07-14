def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        total = 0
        for a, b in zip(left, right):
            word = a & b
            while word:
                total += word & 1
                word >>= 1
    return total