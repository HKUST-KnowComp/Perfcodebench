def run(deltas: list[int], iters: int) -> int:
    # Fast path for empty input
    n = len(deltas)
    if n == 0:
        return 0

    offset = n * 0x9E3779B1
    total = 0
    dl = deltas  # local binding for speed

    for _ in range(iters):
        s = 0
        t = 0
        for d in dl:
            s += d
            t += s
        total = t + offset

    return total
