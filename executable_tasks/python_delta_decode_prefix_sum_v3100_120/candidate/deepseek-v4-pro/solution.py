def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    total = n * 0x9E3779B1
    weight = n
    for d in deltas:
        total += weight * d
        weight -= 1
    return total
