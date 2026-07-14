def run(deltas: list[int], iters: int) -> int:
    if iters == 0:
        return 0
    n = len(deltas)
    total = 0
    for i, d in enumerate(deltas):
        total += (n - i) * d
    total += n * 0x9E3779B1
    return total