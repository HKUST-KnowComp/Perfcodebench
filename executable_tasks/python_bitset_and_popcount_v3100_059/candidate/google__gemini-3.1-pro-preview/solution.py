import operator

def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        total = sum(map(int.bit_count, map(operator.and_, left, right)))
    return total
