def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    const_sum = n * 0x9E3779B1
    total = 0
    for _ in range(iters):
        current = 0
        prefix_sum = 0
        for d in deltas:
            current += d
            prefix_sum += current
        total = prefix_sum + const_sum
    return total
