def run(deltas: list[int], iters: int) -> int:
    total = 0
    current = 0
    const = 0x9E3779B1
    for d in deltas:
        current += d
        total += current + const
    return total
