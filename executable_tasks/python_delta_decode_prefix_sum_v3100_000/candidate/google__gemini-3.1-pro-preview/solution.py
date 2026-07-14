def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    n = len(deltas)
    return n * 0x9E3779B1 + sum([d * m for d, m in zip(deltas, range(n, 0, -1))])
