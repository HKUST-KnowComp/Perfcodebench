from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    # Compute sum of prefix sums using C-optimized accumulate
    prefix_sum_total = sum(accumulate(deltas))
    # Add constant offset for each element (len(deltas) * 0x9E3779B1)
    return prefix_sum_total + len(deltas) * 0x9E3779B1