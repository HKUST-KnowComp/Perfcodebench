def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        total = 0
        for a, b in zip(left, right):
            word = a & b
            # Brian Kernighan's algorithm for popcount
            while word:
                word &= word - 1
                total += 1
    return total