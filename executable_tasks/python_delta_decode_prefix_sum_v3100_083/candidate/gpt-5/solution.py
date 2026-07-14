from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    # If no iterations are requested, baseline returns 0
    if iters <= 0:
        return 0
    n = len(deltas)
    if n == 0:
        return 0
    # Sum of decoded prefix values plus constant per element
    total = sum(accumulate(deltas))
    total += 0x9E3779B1 * n
    return total
