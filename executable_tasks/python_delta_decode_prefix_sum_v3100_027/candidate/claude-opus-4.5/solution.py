def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    magic = 0x9E3779B1
    offset = n * magic
    total = 0
    for _ in range(iters):
        current = 0
        prefix_sum = 0
        for d in deltas:
            current += d
            prefix_sum += current
        total = prefix_sum + offset
    return total
