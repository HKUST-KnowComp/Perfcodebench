def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    total = n * 0x9E3779B1
    mult = n
    for d in deltas:
        total += d * mult
        mult -= 1
    return total