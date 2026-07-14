from itertools import accumulate


def run(deltas: list[int], iters: int) -> int:
    # If no iterations are requested, baseline returns 0
    if iters <= 0:
        return 0

    n = len(deltas)
    if n == 0:
        return 0

    const = 0x9E3779B1
    # Sum of decoded prefix values plus constant per element
    prefix_sum_total = sum(accumulate(deltas))
    return prefix_sum_total + n * const
