def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    offset = 0x9E3779B1
    total = 0
    for _ in range(iters):
        current = 0
        iteration_sum = 0
        for d in deltas:
            current += d
            iteration_sum += current + offset
        total = iteration_sum
    return total