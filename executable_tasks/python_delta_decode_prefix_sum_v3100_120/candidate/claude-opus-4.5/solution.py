def run(deltas: list[int], iters: int) -> int:
    n = len(deltas)
    if n == 0:
        return 0
    offset = 0x9E3779B1
    # Only the last iteration's total matters (total is overwritten each iter)
    # Compute prefix sums and their sum in a single pass
    current = 0
    total = 0
    for d in deltas:
        current += d
        total += current
    total += n * offset
    return total
