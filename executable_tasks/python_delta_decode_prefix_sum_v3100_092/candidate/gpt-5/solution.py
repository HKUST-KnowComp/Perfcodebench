def run(deltas: list[int], iters: int) -> int:
    # Preserve baseline behavior: when iters <= 0, return 0
    if iters <= 0:
        return 0

    n = len(deltas)
    if n == 0:
        return 0

    cur = 0
    prefix_sum_total = 0
    arr = deltas  # local binding for speed

    # Compute sum of prefix sums in one pass
    for d in arr:
        cur += d
        prefix_sum_total += cur

    # Add the per-element constant once
    return prefix_sum_total + n * 0x9E3779B1
