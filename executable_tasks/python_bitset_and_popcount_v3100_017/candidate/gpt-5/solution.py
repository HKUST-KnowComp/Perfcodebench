def run(left: list[int], right: list[int], iters: int) -> int:
    # Compute the popcount of the bitwise AND across corresponding 64-bit words.
    # Semantics match the baseline: only consider the lower 64 bits of each word.
    n = min(len(left), len(right))
    if n == 0:
        return 0
    L = left
    R = right
    m = (1 << 64) - 1
    bc = int.bit_count
    total = 0
    i = 0
    while i < n:
        total += bc((L[i] & R[i]) & m)
        i += 1
    return total
