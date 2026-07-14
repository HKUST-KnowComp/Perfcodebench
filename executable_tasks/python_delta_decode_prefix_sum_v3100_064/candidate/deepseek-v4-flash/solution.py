def run(deltas: list[int], iters: int) -> int:
    total_one = 0
    current = 0
    for d in deltas:
        current += d
        total_one += current + 0x9E3779B1
    return total_one * iters