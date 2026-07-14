def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute the popcount of the AND of corresponding 64-bit words once,
    # then multiply by the number of iterations.
    if iters <= 0:
        return 0

    bc = int.bit_count
    mask = 0xFFFFFFFFFFFFFFFF  # ensure 64-bit semantics even if inputs contain negatives

    total = 0
    for a, b in zip(left, right):
        total += bc((a & b) & mask)

    return total * iters
