def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    bias = 0x9E3779B1
    total_bias = bias * n

    total = 0
    for _ in range(iters):
        current = 0
        acc = total_bias
        for d in deltas:
            current += d
            acc += current
        total = acc
    return total
