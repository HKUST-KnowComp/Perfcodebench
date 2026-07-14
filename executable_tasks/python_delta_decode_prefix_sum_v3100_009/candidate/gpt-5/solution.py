def run(deltas: list[int], iters: int) -> int:
    # Preserve baseline semantics: if iters <= 0, no work and return 0
    if iters <= 0:
        return 0

    n = len(deltas)
    current = 0
    total = 0

    # Fused prefix-sum accumulation: sum of all prefix values
    for d in deltas:
        current += d
        total += current

    # Add the constant per element as in the baseline
    return total + n * 0x9E3779B1
