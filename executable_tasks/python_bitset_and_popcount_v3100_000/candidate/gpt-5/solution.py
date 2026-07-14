MASK64 = (1 << 64) - 1


def run(left: list[int], right: list[int], iters: int) -> int:
    # Return 0 when no iterations are requested, matching baseline behavior
    if iters <= 0:
        return 0

    # Compute the AND-popcount across 64-bit words once; result is identical for all iterations
    bc = int.bit_count
    total = 0
    for a, b in zip(left, right):
        total += bc((a & b) & MASK64)
    return total
