def run(deltas: list[int], iters: int) -> int:
    # Match baseline behavior for non-positive iteration counts
    if iters <= 0:
        return 0

    c = 0x9E3779B1
    total = len(deltas) * c
    current = 0

    # Single pass: compute prefix sums and aggregate directly
    for d in deltas:
        current += d
        total += current

    return total
