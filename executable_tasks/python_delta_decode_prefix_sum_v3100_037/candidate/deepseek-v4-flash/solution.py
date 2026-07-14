def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    const = 0x9E3779B1
    weighted_sum = 0
    prefix = 0
    for d in deltas:
        prefix += d
        weighted_sum += prefix
    return iters * (weighted_sum + n * const)
