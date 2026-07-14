def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    const_term = n * 0x9E3779B1
    total = 0
    for _ in range(iters):
        s = 0
        cur = 0
        for d in deltas:
            cur += d
            s += cur
        total = s + const_term
    return total