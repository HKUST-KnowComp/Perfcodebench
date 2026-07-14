def run(deltas: list[int], iters: int) -> int:
    if iters == 0:
        return 0
    n = len(deltas)
    total = 0
    weight = n
    for d in deltas:
        total += d * weight
        weight -= 1
    return total + n * 0x9E3779B1
