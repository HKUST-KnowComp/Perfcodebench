def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        count = 0
        for a, b in zip(left, right):
            word = a & b
            while word:
                word &= word - 1
                count += 1
        total = count
    return total
