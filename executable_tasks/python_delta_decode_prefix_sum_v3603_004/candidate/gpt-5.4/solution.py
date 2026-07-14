def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0 or iters <= 0:
        return 0

    const = 0x9E3779B1
    total = 0
    ds = deltas

    for _ in range(iters):
        current = 0
        s = 0
        for d in ds:
            current += d
            s += current
        total = s + const * n

    return total
