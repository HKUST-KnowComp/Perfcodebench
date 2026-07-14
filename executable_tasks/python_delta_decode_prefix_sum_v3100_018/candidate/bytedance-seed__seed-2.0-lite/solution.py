def run(deltas: list[int], iters: int) -> int:
    current = 0
    sum_prefix = 0
    n = len(deltas)
    for d in deltas:
        current += d
        sum_prefix += current
    const = 0x9E3779B1
    per_iter_total = sum_prefix + n * const
    return per_iter_total * iters