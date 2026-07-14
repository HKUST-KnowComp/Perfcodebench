def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    const = 0x9E3779B1
    const_sum = n * const
    total = 0
    for _ in range(iters):
        prefix = 0
        prefix_sum = 0
        for d in deltas:
            prefix += d
            prefix_sum += prefix
        total = prefix_sum + const_sum
    return total
