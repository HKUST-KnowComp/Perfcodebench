def run(deltas: list[int], iters: int) -> int:
    # If no iterations are requested, behavior matches baseline: return 0.
    if iters <= 0:
        return 0

    # Compute the sum of prefix sums in a single pass, then add the constant per element.
    current = 0
    total = 0
    n = len(deltas)

    # Fused delta decoding and aggregation
    for d in deltas:
        current += d
        total += current

    # Add the constant for each element once
    total += n * 0x9E3779B1

    return total
