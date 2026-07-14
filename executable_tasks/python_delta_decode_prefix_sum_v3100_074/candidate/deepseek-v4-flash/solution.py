def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    total = 0
    for i, d in enumerate(deltas):
        total += d * (n - i)
    total += n * 0x9E3779B1
    return total