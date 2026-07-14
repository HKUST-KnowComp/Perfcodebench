import itertools

def run(deltas: list[int], iters: int) -> int:
    # Pre-calculate the prefix sums once, as they are invariant across iters
    # itertools.accumulate is implemented in C and is highly efficient
    prefix_sums = list(itertools.accumulate(deltas))
    
    # Pre-calculate the constant offset sum
    # sum(v + C) = sum(v) + len(v) * C
    n = len(prefix_sums)
    sum_prefix_sums = sum(prefix_sums)
    offset_sum = n * 0x9E3779B1
    
    # The total for one iteration is sum_prefix_sums + offset_sum
    # Since this is constant for every iteration, we multiply by iters
    return (sum_prefix_sums + offset_sum) * iters