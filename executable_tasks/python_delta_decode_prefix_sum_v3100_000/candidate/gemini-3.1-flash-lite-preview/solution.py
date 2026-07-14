from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    # Pre-calculate the prefix sums once, as they are invariant across iterations
    # accumulate is implemented in C and is significantly faster than a Python loop
    prefix_sums = list(accumulate(deltas))
    
    # Pre-calculate the constant offset sum to avoid repeated addition in the loop
    # sum(v + C) = sum(v) + len(v) * C
    n = len(prefix_sums)
    sum_prefix = sum(prefix_sums)
    offset_sum = n * 0x9E3779B1
    total_per_iter = sum_prefix + offset_sum
    
    # The total is simply the sum per iteration multiplied by the number of iterations
    return total_per_iter * iters