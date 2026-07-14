def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute popcount of the bitwise-AND of corresponding 64-bit words.
    # Preserve zip truncation behavior and 64-bit semantics of the baseline.
    mask = 0xFFFFFFFFFFFFFFFF
    bc = int.bit_count
    total = 0
    for a, b in zip(left, right):
        total += bc((a & b) & mask)
    return total
