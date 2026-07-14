def run(left: list[int], right: list[int], iters: int) -> int:
    # Preserve baseline semantics: if no iterations, return 0
    if iters <= 0:
        return 0

    # Popcount of AND across 64-bit words
    mask = (1 << 64) - 1
    bc = int.bit_count

    total = 0
    for a, b in zip(left, right):
        total += bc((a & b) & mask)
    return total
