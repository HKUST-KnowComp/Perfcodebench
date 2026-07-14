def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    n = len(deltas)
    s = 0
    w = n
    for d in deltas:
        s += d * w
        w -= 1
    return s + n * 0x9E3779B1