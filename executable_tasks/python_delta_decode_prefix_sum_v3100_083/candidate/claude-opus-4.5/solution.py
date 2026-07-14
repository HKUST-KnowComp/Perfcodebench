from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    prefix_sum = sum(accumulate(deltas))
    total_one_iter = prefix_sum + n * 0x9E3779B1
    return total_one_iter * iters
