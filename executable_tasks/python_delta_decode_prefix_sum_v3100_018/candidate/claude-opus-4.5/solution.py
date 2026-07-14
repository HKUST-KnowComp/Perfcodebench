def run(deltas: list[int], iters: int) -> int:
    if not deltas:
        return 0
    n = len(deltas)
    current = 0
    prefix_sum = 0
    for d in deltas:
        current += d
        prefix_sum += current
    total = prefix_sum + n * 0x9E3779B1
    return total
