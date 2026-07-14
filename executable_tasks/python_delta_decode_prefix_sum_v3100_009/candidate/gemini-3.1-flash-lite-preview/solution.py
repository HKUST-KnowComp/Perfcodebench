def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    offset = 0x9E3779B1
    total = 0
    for _ in range(iters):
        current_sum = 0
        iteration_total = 0
        for d in deltas:
            current_sum += d
            iteration_total += current_sum + offset
        total = iteration_total
    return total