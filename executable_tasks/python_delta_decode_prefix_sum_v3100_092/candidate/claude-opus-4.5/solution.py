def run(deltas: list[int], iters: int) -> int:
    if iters == 0:
        return 0
    n = len(deltas)
    if n == 0:
        return 0
    # Compute prefix sums and their total in one pass
    current = 0
    prefix_sum_total = 0
    for d in deltas:
        current += d
        prefix_sum_total += current
    # Each iteration adds the same total plus n * 0x9E3779B1
    single_iter_total = prefix_sum_total + n * 0x9E3779B1
    return single_iter_total * iters
