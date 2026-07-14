def run(deltas: list[int], iters: int) -> int:
    # Preserve baseline semantics: return 0 if no iterations are performed
    if iters <= 0:
        return 0

    dl = deltas
    n = len(dl)

    cur = 0
    total = 0
    # Decode deltas into prefix sums and accumulate their sum in one pass
    for d in dl:
        cur += d
        total += cur

    # Add the constant contribution once
    return total + 0x9E3779B1 * n
