def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    const = 0x9E3779B1
    # sum of prefix sums equals sum(d[i] * (n - i))
    total = 0
    w = 1
    for d in reversed(deltas):
        total += d * w
        w += 1
    total += const * n
    return total
