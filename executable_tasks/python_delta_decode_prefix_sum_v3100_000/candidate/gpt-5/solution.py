def run(deltas: list[int], iters: int) -> int:
    if iters <= 0:
        return 0
    total = 0
    current = 0
    K = 0x9E3779B1
    for d in deltas:
        current += d
        total += current
        total += K
    return total
