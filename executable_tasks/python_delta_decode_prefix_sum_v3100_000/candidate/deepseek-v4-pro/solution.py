def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    total = 0
    current = 0
    for d in deltas:
        current += d
        total += current
    total += n * 0x9E3779B1
    return total
