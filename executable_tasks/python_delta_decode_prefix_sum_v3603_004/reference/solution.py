def run(deltas: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        current = 0
        total = 0
        for d in deltas:
            current += d
            total += current + 0x9E3779B1
    return total
