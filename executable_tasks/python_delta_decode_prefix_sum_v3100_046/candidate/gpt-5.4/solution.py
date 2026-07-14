def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0

    bias_total = 0x9E3779B1 * n
    total = 0

    for _ in range(iters):
        current = 0
        s = 0
        for d in deltas:
            current += d
            s += current
        total = s + bias_total

    return total
