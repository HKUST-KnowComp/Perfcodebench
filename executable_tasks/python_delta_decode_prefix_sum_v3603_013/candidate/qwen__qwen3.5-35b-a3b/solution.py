def run(deltas: list[int], iters: int) -> int:
    values = [0] * len(deltas)
    current = 0
    for i, d in enumerate(deltas):
        current += d
        values[i] = current
    total = 0
    for v in values:
        total += v + 0x9E3779B1
    return total