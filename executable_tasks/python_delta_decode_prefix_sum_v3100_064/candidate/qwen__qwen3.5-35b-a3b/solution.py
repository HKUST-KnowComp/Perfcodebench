from itertools import accumulate

def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    # The baseline resets 'total' inside the 'iters' loop, so only the last iteration's result matters.
    # Since 'deltas' is constant, the result is identical for any iters >= 1.
    # We compute the sum of prefix sums once using C-optimized accumulate.
    prefix_sum_total = sum(accumulate(deltas))
    # Add the constant offset for each element (len(deltas) * 0x9E3779B1)
    return prefix_sum_total + len(deltas) * 0x9E3779B1