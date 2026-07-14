def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute the popcount of the bitwise AND across corresponding 64-bit words.
    # Only the low 64 bits are considered, matching baseline semantics.
    n = min(len(left), len(right))
    if n == 0:
        return 0

    MASK64 = 0xFFFFFFFFFFFFFFFF
    la = left
    ra = right
    bc = int.bit_count

    total = 0
    for i in range(n):
        total += bc((la[i] & ra[i]) & MASK64)
    return total
