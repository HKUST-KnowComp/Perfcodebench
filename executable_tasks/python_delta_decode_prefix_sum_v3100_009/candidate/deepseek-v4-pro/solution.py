def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    n = len(deltas)
    constant = 0x9E3779B1
    total_sum = 0
    for j, d in enumerate(deltas):
        total_sum += d * (n - j)
    return total_sum + n * constant
