def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute the popcount of the bitwise AND across corresponding 64-bit words.
    # Only the lower 64 bits are counted, matching the baseline behavior.
    mask = (1 << 64) - 1
    bc = int.bit_count

    l = left
    r = right
    n = len(l)
    m = len(r)
    if m < n:
        n = m

    total = 0
    i = 0
    while i < n:
        total += bc((l[i] & r[i]) & mask)
        i += 1

    return total
