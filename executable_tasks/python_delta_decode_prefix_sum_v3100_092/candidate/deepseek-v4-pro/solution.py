def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    current = 0
    sum_prefix = 0
    for d in deltas:
        current += d
        sum_prefix += current
    return sum_prefix + len(deltas) * 0x9E3779B1
