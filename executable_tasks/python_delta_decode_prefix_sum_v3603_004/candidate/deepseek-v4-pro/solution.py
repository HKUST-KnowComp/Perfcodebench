def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    n = len(deltas)
    constant = 0x9E3779B1
    total = n * constant
    s = 0
    for i, d in enumerate(deltas):
        s += (n - i) * d
    return s + total
