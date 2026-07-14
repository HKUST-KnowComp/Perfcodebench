def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    total = 0
    for j, d in enumerate(deltas):
        total += d * (n - j)
    total += n * 0x9E3779B1
    return total
