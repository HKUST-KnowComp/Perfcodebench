def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    c = 0x9E3779B1
    base = n * c
    total = 0
    for _ in range(iters):
        s = 0
        for i, d in enumerate(deltas):
            s += d * (n - i)
        total = s + base
    return total