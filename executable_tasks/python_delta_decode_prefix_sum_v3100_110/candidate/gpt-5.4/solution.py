def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0

    ds = deltas
    bias_total = n * 0x9E3779B1
    total = 0

    for _ in range(iters):
        current = 0
        s = 0
        for d in ds:
            current += d
            s += current
        total = s + bias_total

    return total
