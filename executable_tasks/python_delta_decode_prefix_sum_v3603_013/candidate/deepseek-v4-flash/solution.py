def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    n = len(deltas)
    total_prefix = 0
    weight = n
    for d in deltas:
        total_prefix += weight * d
        weight -= 1
    constant = 0x9E3779B1
    return total_prefix + n * constant