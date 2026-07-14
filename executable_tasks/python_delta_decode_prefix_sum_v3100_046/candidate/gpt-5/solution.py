def run(deltas: list[int], iters: int) -> int:
    # Preserve baseline semantics: if no iterations, return 0
    if iters <= 0:
        return 0

    n = len(deltas)
    if n == 0:
        return 0

    current = 0
    s = 0
    for d in deltas:
        current += d
        s += current

    return s + n * 0x9E3779B1
