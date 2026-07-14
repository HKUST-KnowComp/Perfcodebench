def run(deltas: list[int], iters: int) -> int:
    # Preserve baseline semantics: if iters == 0, no work and return 0.
    if iters <= 0:
        return 0

    # One pass: compute sum of prefix sums plus constant per element.
    n = len(deltas)
    total = n * 0x9E3779B1
    current = 0
    for d in deltas:
        current += d
        total += current
    return total
