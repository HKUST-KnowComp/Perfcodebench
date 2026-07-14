def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    return sum(d * (n - i) for i, d in enumerate(deltas)) + n * 0x9E3779B1