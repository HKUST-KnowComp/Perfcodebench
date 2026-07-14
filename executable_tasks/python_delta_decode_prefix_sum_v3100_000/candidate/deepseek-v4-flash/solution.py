def run(deltas: list[int], iters: int) -> int:
    const = 0x9E3779B1
    n = len(deltas)
    cur = 0
    total = 0
    for d in deltas:
        cur += d
        total += cur
    total += n * const
    return total