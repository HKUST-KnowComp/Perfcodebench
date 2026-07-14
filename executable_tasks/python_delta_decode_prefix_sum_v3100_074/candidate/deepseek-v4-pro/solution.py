def run(deltas: list[int], iters: int) -> int:
    if iters == 0:
        return 0
    C = 0x9E3779B1
    total = 0
    prefix = 0
    for d in deltas:
        prefix += d
        total += prefix
    total += len(deltas) * C
    return total