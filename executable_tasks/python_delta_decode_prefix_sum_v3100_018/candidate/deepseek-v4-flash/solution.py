def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    sum_prefix = 0
    current = 0
    for d in deltas:
        current += d
        sum_prefix += current
    const = 0x9E3779B1
    total_per_iter = sum_prefix + n * const
    return iters * total_per_iter