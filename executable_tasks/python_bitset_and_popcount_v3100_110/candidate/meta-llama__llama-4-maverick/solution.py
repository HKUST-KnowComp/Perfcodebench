def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for a, b in zip(left, right):
        word = a & b
        total += bin(word).count('1')
    return total