from itertools import accumulate

C = 0x9E3779B1


def run(deltas: list[int], iters: int) -> int:
    # Match baseline behavior: if no iterations, return 0
    if iters <= 0:
        return 0

    n = len(deltas)
    if n == 0:
        return 0

    # Sum of prefix sums via C-level accumulate + sum
    total_prefix_sum = sum(accumulate(deltas))
    return total_prefix_sum + n * C
