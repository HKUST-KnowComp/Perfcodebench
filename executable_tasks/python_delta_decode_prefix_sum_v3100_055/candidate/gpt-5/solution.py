def run(deltas: list[int], iters: int) -> int:
    current = 0
    total = 0
    for d in deltas:
        current += d
        total += current
    return total + 0x9E3779B1 * len(deltas)
